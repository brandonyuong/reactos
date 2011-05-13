/*
 * PROJECT:     ReactOS Universal Serial Bus Bulk Storage Driver
 * LICENSE:     GPL - See COPYING in the top level directory
 * FILE:        drivers/usb/usbstor/pdo.c
 * PURPOSE:     USB block storage device driver.
 * PROGRAMMERS:
 *              James Tabor
 *              Michael Martin (michael.martin@reactos.org)
 *              Johannes Anderwald (johannes.anderwald@reactos.org)
 */

#include "usbstor.h"

NTSTATUS
USBSTOR_BuildCBW(
    IN ULONG Tag,
    IN ULONG DataTransferLength,
    IN UCHAR LUN,
    IN UCHAR CommandBlockLength,
    IN PUCHAR CommandBlock,
    IN OUT PCBW Control)
{
    //
    // sanity check
    //
    ASSERT(CommandBlockLength <= 16);

    //
    // now initialize CBW
    //
    Control->Signature = CBW_SIGNATURE;
    Control->Tag = Tag;
    Control->DataTransferLength = DataTransferLength;
    Control->Flags = 0x80;
    Control->LUN = (LUN & MAX_LUN);
    Control->CommandBlockLength = CommandBlockLength;

    //
    // copy command block
    //
    RtlCopyMemory(Control->CommandBlock, CommandBlock, CommandBlockLength);

    //
    // done
    //
    return STATUS_SUCCESS;
}

PIRP_CONTEXT
USBSTOR_AllocateIrpContext()
{
    PIRP_CONTEXT Context;

    //
    // allocate irp context
    //
    Context = (PIRP_CONTEXT)AllocateItem(NonPagedPool, sizeof(IRP_CONTEXT));
    if (!Context)
    {
        //
        // no memory
        //
        return NULL;
    }

    //
    // allocate cbw block
    //
    Context->cbw = (PCBW)AllocateItem(NonPagedPool, 512);
    if (!Context->cbw)
    {
        //
        // no memory
        //
        FreeItem(Context);
        return NULL;
    }

    //
    // done
    //
    return Context;

}

//
// driver verifier
//
IO_COMPLETION_ROUTINE USBSTOR_CSWCompletionRoutine;

NTSTATUS
NTAPI
USBSTOR_CSWCompletionRoutine(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp, 
    PVOID Ctx)
{
    PIRP_CONTEXT Context;
    PIO_STACK_LOCATION IoStack;
    PSCSI_REQUEST_BLOCK Request;
    PCDB pCDB;
    PREAD_CAPACITY_DATA_EX CapacityDataEx;
    PREAD_CAPACITY_DATA CapacityData;
    PUFI_CAPACITY_RESPONSE Response;

    DPRINT1("USBSTOR_CSWCompletionRoutine Irp %p Ctx %p\n", Irp, Ctx);

    //
    // access context
    //
    Context = (PIRP_CONTEXT)Ctx;

    if (Context->TransferBufferMDL)
    {
        //
        // free mdl
        //
        IoFreeMdl(Context->TransferBufferMDL);
    }

    if (Context->Irp)
    {
        //
        // get current stack location
        //
        IoStack = IoGetCurrentIrpStackLocation(Context->Irp);

        //
        // get request block
        //
        Request = (PSCSI_REQUEST_BLOCK)IoStack->Parameters.Others.Argument1;
        ASSERT(Request);

        //
        // FIXME: check status
        //
        Request->SrbStatus = SRB_STATUS_SUCCESS;

        //
        // get SCSI command data block
        //
        pCDB = (PCDB)Request->Cdb;

        //
        // read capacity needs special work
        //
        if (pCDB->AsByte[0] == SCSIOP_READ_CAPACITY)
        {
            //
            // get output buffer
            //
            Response = (PUFI_CAPACITY_RESPONSE)Context->TransferData;

            //
            // store in pdo
            //
            Context->PDODeviceExtension->BlockLength = NTOHL(Response->BlockLength);
            Context->PDODeviceExtension->LastLogicBlockAddress = NTOHL(Response->LastLogicalBlockAddress);

            if (Request->DataTransferLength == sizeof(READ_CAPACITY_DATA_EX))
            {
                //
                // get input buffer
                //
                CapacityDataEx = (PREAD_CAPACITY_DATA_EX)Request->DataBuffer;

                //
                // set result
                //
                CapacityDataEx->BytesPerBlock = Response->BlockLength;
                CapacityDataEx->LogicalBlockAddress.QuadPart = Response->LastLogicalBlockAddress;
                Irp->IoStatus.Information = sizeof(READ_CAPACITY_DATA_EX);
            }
            else
            {
                //
                // get input buffer
                //
                CapacityData = (PREAD_CAPACITY_DATA)Request->DataBuffer;

                //
                // set result
                //
                CapacityData->BytesPerBlock = Response->BlockLength;
                CapacityData->LogicalBlockAddress = Response->LastLogicalBlockAddress;
                Irp->IoStatus.Information = sizeof(READ_CAPACITY_DATA);
            }

            //
            // free response
            //
            FreeItem(Context->TransferData);
        }
    }

    //
    // free cbw
    //
    FreeItem(Context->cbw);


    if (Context->Irp)
    {
        //
        // FIXME: check status
        //
        Context->Irp->IoStatus.Status = Irp->IoStatus.Status;
        Context->Irp->IoStatus.Information = Context->TransferDataLength;

        //
        // complete request
        //
        IoCompleteRequest(Context->Irp, IO_NO_INCREMENT);
    }

    if (Context->Event)
    {
        //
        // signal event
        //
        KeSetEvent(Context->Event, 0, FALSE);
    }


    //
    // free context
    //
    FreeItem(Context);

    //
    // done
    //
    return STATUS_MORE_PROCESSING_REQUIRED;
}

//
// driver verifier
//
IO_COMPLETION_ROUTINE USBSTOR_DataCompletionRoutine;

NTSTATUS
NTAPI
USBSTOR_DataCompletionRoutine(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp, 
    PVOID Ctx)
{
    PIRP_CONTEXT Context;
    PIO_STACK_LOCATION IoStack;

    DPRINT1("USBSTOR_DataCompletionRoutine Irp %p Ctx %p\n", Irp, Ctx);

    //
    // access context
    //
    Context = (PIRP_CONTEXT)Ctx;

    //
    // get next stack location
    //

    IoStack = IoGetNextIrpStackLocation(Irp);

    //
    // now initialize the urb for sending the csw
    //
    UsbBuildInterruptOrBulkTransferRequest(&Context->Urb,
                                           sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),
                                           Context->FDODeviceExtension->InterfaceInformation->Pipes[Context->FDODeviceExtension->BulkInPipeIndex].PipeHandle,
                                           Context->csw,
                                           NULL,
                                           512, //FIXME
                                           USBD_TRANSFER_DIRECTION_IN | USBD_SHORT_TRANSFER_OK,
                                           NULL);

    //
    // initialize stack location
    //
    IoStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
    IoStack->Parameters.DeviceIoControl.IoControlCode = IOCTL_INTERNAL_USB_SUBMIT_URB;
    IoStack->Parameters.Others.Argument1 = (PVOID)&Context->Urb;
    IoStack->Parameters.DeviceIoControl.InputBufferLength = Context->Urb.UrbHeader.Length;
    Irp->IoStatus.Status = STATUS_SUCCESS;


    //
    // setup completion routine
    //
    IoSetCompletionRoutine(Irp, USBSTOR_CSWCompletionRoutine, Context, TRUE, TRUE, TRUE);

    //
    // call driver
    //
    IoCallDriver(Context->FDODeviceExtension->LowerDeviceObject, Irp);

    return STATUS_MORE_PROCESSING_REQUIRED;
}

//
// driver verifier
//
IO_COMPLETION_ROUTINE USBSTOR_CBWCompletionRoutine;

NTSTATUS
NTAPI
USBSTOR_CBWCompletionRoutine(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp, 
    PVOID Ctx)
{
    PIRP_CONTEXT Context;
    PIO_STACK_LOCATION IoStack;

    DPRINT1("USBSTOR_CBWCompletionRoutine Irp %p Ctx %p\n", Irp, Ctx);

    //
    // access context
    //
    Context = (PIRP_CONTEXT)Ctx;


    //
    // get next stack location
    //
    IoStack = IoGetNextIrpStackLocation(Irp);

    //
    // is there data to be submitted
    //
    if (Context->TransferDataLength)
    {
        //
        // now initialize the urb for sending data
        //

        UsbBuildInterruptOrBulkTransferRequest(&Context->Urb,
                                               sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),
                                               Context->FDODeviceExtension->InterfaceInformation->Pipes[Context->FDODeviceExtension->BulkInPipeIndex].PipeHandle,
                                               NULL,
                                               Context->TransferBufferMDL,
                                               Context->TransferDataLength,
                                               USBD_TRANSFER_DIRECTION_IN | USBD_SHORT_TRANSFER_OK,
                                               NULL);

        //
        // setup completion routine
        //
        IoSetCompletionRoutine(Irp, USBSTOR_DataCompletionRoutine, Context, TRUE, TRUE, TRUE);
    }
    else
    {
        //
        // now initialize the urb for sending the csw
        //

        UsbBuildInterruptOrBulkTransferRequest(&Context->Urb,
                                               sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),
                                               Context->FDODeviceExtension->InterfaceInformation->Pipes[Context->FDODeviceExtension->BulkInPipeIndex].PipeHandle,
                                               Context->csw,
                                               NULL,
                                               512, //FIXME
                                               USBD_TRANSFER_DIRECTION_IN | USBD_SHORT_TRANSFER_OK,
                                               NULL);

        //
        // setup completion routine
        //
        IoSetCompletionRoutine(Irp, USBSTOR_CSWCompletionRoutine, Context, TRUE, TRUE, TRUE);
    }

    //
    // initialize stack location
    //
    IoStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
    IoStack->Parameters.DeviceIoControl.IoControlCode = IOCTL_INTERNAL_USB_SUBMIT_URB;
    IoStack->Parameters.Others.Argument1 = (PVOID)&Context->Urb;
    IoStack->Parameters.DeviceIoControl.InputBufferLength = Context->Urb.UrbHeader.Length;
    Irp->IoStatus.Status = STATUS_SUCCESS;

    //
    // call driver
    //
    IoCallDriver(Context->FDODeviceExtension->LowerDeviceObject, Irp);

    return STATUS_MORE_PROCESSING_REQUIRED;
}

NTSTATUS
USBSTOR_SendRequest(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP OriginalRequest,
    IN OPTIONAL PKEVENT Event,
    IN ULONG CommandLength,
    IN PUCHAR Command,
    IN ULONG TransferDataLength,
    IN PUCHAR TransferData)
{
    PIRP_CONTEXT Context;
    PPDO_DEVICE_EXTENSION PDODeviceExtension;
    PFDO_DEVICE_EXTENSION FDODeviceExtension;
    PIRP Irp;
    PIO_STACK_LOCATION IoStack;

    //
    // first allocate irp context
    //
    Context = USBSTOR_AllocateIrpContext();
    if (!Context)
    {
        //
        // no memory
        //
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // get PDO device extension
    //
    PDODeviceExtension = (PPDO_DEVICE_EXTENSION)DeviceObject->DeviceExtension;

    //
    // get FDO device extension
    //
    FDODeviceExtension = (PFDO_DEVICE_EXTENSION)PDODeviceExtension->LowerDeviceObject->DeviceExtension;

    //
    // now build the cbw
    //
    USBSTOR_BuildCBW(0xDEADDEAD, // FIXME tag
                     TransferDataLength,
                     PDODeviceExtension->LUN,
                     CommandLength,
                     Command,
                     Context->cbw);

    //
    // now initialize the urb
    //
    UsbBuildInterruptOrBulkTransferRequest(&Context->Urb,
                                           sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER),
                                           FDODeviceExtension->InterfaceInformation->Pipes[FDODeviceExtension->BulkOutPipeIndex].PipeHandle,
                                           Context->cbw,
                                           NULL,
                                           sizeof(CBW),
                                           USBD_TRANSFER_DIRECTION_OUT | USBD_SHORT_TRANSFER_OK,
                                           NULL);

    //
    // initialize rest of context
    //
    Context->Irp = OriginalRequest;
    Context->TransferData = TransferData;
    Context->TransferDataLength = TransferDataLength;
    Context->FDODeviceExtension = FDODeviceExtension;
    Context->PDODeviceExtension = PDODeviceExtension;
    Context->Event = Event;

    //
    // is there transfer data
    //
    if (Context->TransferDataLength)
    {
        //
        // allocate mdl for buffer, buffer must be allocated from NonPagedPool
        //
        Context->TransferBufferMDL = IoAllocateMdl(Context->TransferData, Context->TransferDataLength, FALSE, FALSE, NULL);
        if (!Context->TransferBufferMDL)
        {
            //
            // failed to allocate MDL
            //
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        //
        // build mdl for nonpaged pool
        //
        MmBuildMdlForNonPagedPool(Context->TransferBufferMDL);
    }

    //
    // now allocate the request
    //
    Irp = IoAllocateIrp(DeviceObject->StackSize, FALSE);
    if (!Irp)
    {
        FreeItem(Context->cbw);
        FreeItem(Context);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // get next stack location
    //
    IoStack = IoGetNextIrpStackLocation(Irp);

    //
    // initialize stack location
    //
    IoStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
    IoStack->Parameters.DeviceIoControl.IoControlCode = IOCTL_INTERNAL_USB_SUBMIT_URB;
    IoStack->Parameters.Others.Argument1 = (PVOID)&Context->Urb;
    IoStack->Parameters.DeviceIoControl.InputBufferLength = Context->Urb.UrbHeader.Length;
    Irp->IoStatus.Status = STATUS_SUCCESS;

    //
    // setup completion routine
    //
    IoSetCompletionRoutine(Irp, USBSTOR_CBWCompletionRoutine, Context, TRUE, TRUE, TRUE);

    if (OriginalRequest)
    {
        //
        // mark orignal irp as pending
        //
        IoMarkIrpPending(OriginalRequest);
    }

    //
    // call driver
    //
    IoCallDriver(FDODeviceExtension->LowerDeviceObject, Irp);

    //
    // done
    //
    return STATUS_PENDING;
}

NTSTATUS
USBSTOR_SendInquiryCmd(
    IN PDEVICE_OBJECT DeviceObject)
{
    UFI_INQUIRY_CMD Cmd;
    NTSTATUS Status;
    KEVENT Event;
    PPDO_DEVICE_EXTENSION PDODeviceExtension;
    PUFI_INQUIRY_RESPONSE Response;


    //
    // allocate inquiry response
    //
    Response = AllocateItem(NonPagedPool, PAGE_SIZE);
    if (!Response)
    {
        //
        // no memory
        //
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // get PDO device extension
    //
    PDODeviceExtension = (PPDO_DEVICE_EXTENSION)DeviceObject->DeviceExtension;

    //
    // initialize inquiry cmd
    //
    RtlZeroMemory(&Cmd, sizeof(UFI_INQUIRY_CMD));
    Cmd.Code = SCSIOP_INQUIRY;
    Cmd.LUN = (PDODeviceExtension->LUN & MAX_LUN);
    Cmd.AllocationLength = sizeof(UFI_INQUIRY_RESPONSE);

    //
    // initialize event
    //
    KeInitializeEvent(&Event, NotificationEvent, FALSE);

    //
    // now send the request
    //
    Status = USBSTOR_SendRequest(DeviceObject, NULL, &Event, UFI_INQUIRY_CMD_LEN, (PUCHAR)&Cmd, sizeof(UFI_INQUIRY_RESPONSE), (PUCHAR)Response);

    //
    // wait for the action to complete
    //
    KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, NULL);

    DPRINT1("Response %p\n", Response);
    DPRINT1("DeviceType %x\n", Response->DeviceType);
    DPRINT1("RMB %x\n", Response->RMB);
    DPRINT1("Version %x\n", Response->Version);
    DPRINT1("Format %x\n", Response->Format);
    DPRINT1("Length %x\n", Response->Length);
    DPRINT1("Reserved %x\n", Response->Reserved);
    DPRINT1("Vendor %c%c%c%c%c%c%c%c\n", Response->Vendor[0], Response->Vendor[1], Response->Vendor[2], Response->Vendor[3], Response->Vendor[4], Response->Vendor[5], Response->Vendor[6], Response->Vendor[7]);
    DPRINT1("Product %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", Response->Product[0], Response->Product[1], Response->Product[2], Response->Product[3],
                                                          Response->Product[4], Response->Product[5], Response->Product[6], Response->Product[7], 
                                                          Response->Product[8], Response->Product[9], Response->Product[10], Response->Product[11],
                                                          Response->Product[12], Response->Product[13], Response->Product[14], Response->Product[15]);

    DPRINT1("Revision %c%c%c%c\n", Response->Revision[0], Response->Revision[1], Response->Revision[2], Response->Revision[3]);

    //
    // store inquiry data
    //
    PDODeviceExtension->InquiryData = (PVOID)Response;

    //
    // done
    //
    return Status;
}

NTSTATUS
USBSTOR_SendCapacityCmd(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp)
{
    UFI_CAPACITY_CMD Cmd;
    PUFI_CAPACITY_RESPONSE Response;
    PPDO_DEVICE_EXTENSION PDODeviceExtension;

    //
    // get PDO device extension
    //
    PDODeviceExtension = (PPDO_DEVICE_EXTENSION)DeviceObject->DeviceExtension;

    //
    // allocate capacity response
    //
    Response = (PUFI_CAPACITY_RESPONSE)AllocateItem(NonPagedPool, sizeof(UFI_CAPACITY_RESPONSE));
    if (!Response)
    {
        //
        // no memory
        //
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // initialize capacity cmd
    //
    RtlZeroMemory(&Cmd, sizeof(UFI_INQUIRY_CMD));
    Cmd.Code = SCSIOP_READ_CAPACITY;
    Cmd.LUN = (PDODeviceExtension->LUN & MAX_LUN);

    //
    // send request, response will be freed in completion routine
    //
    return USBSTOR_SendRequest(DeviceObject, Irp, NULL, UFI_INQUIRY_CMD_LEN, (PUCHAR)&Cmd, sizeof(UFI_CAPACITY_RESPONSE), (PUCHAR)Response);
}

NTSTATUS
USBSTOR_SendModeSenseCmd(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp)
{
    UFI_SENSE_CMD Cmd;
    NTSTATUS Status;
    PVOID Response;
    PPDO_DEVICE_EXTENSION PDODeviceExtension;
    PCBW OutControl;
    PCDB pCDB;
    PUFI_MODE_PARAMETER_HEADER Header;

    ASSERT(FALSE);
    return STATUS_NOT_IMPLEMENTED;

#if 0
    //
    // get SCSI command data block
    //
    pCDB = (PCDB)Request->Cdb;

    //
    // get PDO device extension
    //
    PDODeviceExtension = (PPDO_DEVICE_EXTENSION)DeviceObject->DeviceExtension;

    //
    // allocate sense response from non paged pool
    //
    Response = (PUFI_CAPACITY_RESPONSE)AllocateItem(NonPagedPool, Request->DataTransferLength);
    if (!Response)
    {
        //
        // no memory
        //
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // sanity check
    //


    // Supported pages
    // MODE_PAGE_ERROR_RECOVERY
    // MODE_PAGE_FLEXIBILE
    // MODE_PAGE_LUN_MAPPING
    // MODE_PAGE_FAULT_REPORTING
    // MODE_SENSE_RETURN_ALL

    //
    // initialize mode sense cmd
    //
    RtlZeroMemory(&Cmd, sizeof(UFI_INQUIRY_CMD));
    Cmd.Code = SCSIOP_MODE_SENSE;
    Cmd.LUN = (PDODeviceExtension->LUN & MAX_LUN);
    Cmd.PageCode = pCDB->MODE_SENSE.PageCode;
    Cmd.PC = pCDB->MODE_SENSE.Pc;
    Cmd.AllocationLength = HTONS(pCDB->MODE_SENSE.AllocationLength);

    DPRINT1("PageCode %x\n", pCDB->MODE_SENSE.PageCode);
    DPRINT1("PC %x\n", pCDB->MODE_SENSE.Pc);

    //
    // now send mode sense cmd
    //
    Status = USBSTOR_SendCBW(DeviceObject, UFI_SENSE_CMD_LEN, (PUCHAR)&Cmd, Request->DataTransferLength, &OutControl);
    if (!NT_SUCCESS(Status))
    {
        //
        // failed to send CBW
        //
        DPRINT1("USBSTOR_SendCapacityCmd> USBSTOR_SendCBW failed with %x\n", Status);
        FreeItem(Response);
        ASSERT(FALSE);
        return Status;
    }

    //
    // now send data block response
    //
    Status = USBSTOR_SendData(DeviceObject, Request->DataTransferLength, Response);
    if (!NT_SUCCESS(Status))
    {
        //
        // failed to send CBW
        //
        DPRINT1("USBSTOR_SendCapacityCmd> USBSTOR_SendData failed with %x\n", Status);
        FreeItem(Response);
        ASSERT(FALSE);
        return Status;
    }

    Header = (PUFI_MODE_PARAMETER_HEADER)Response;

    //
    // TODO: build layout
    //
    // first struct is the header
    // MODE_PARAMETER_HEADER / _MODE_PARAMETER_HEADER10
    //
    // followed by 
    // MODE_PARAMETER_BLOCK
    //
    // 
    UNIMPLEMENTED

    //
    // send csw
    //
    Status = USBSTOR_SendCSW(DeviceObject, OutControl, 512, &CSW);

    DPRINT1("------------------------\n");
    DPRINT1("CSW %p\n", &CSW);
    DPRINT1("Signature %x\n", CSW.Signature);
    DPRINT1("Tag %x\n", CSW.Tag);
    DPRINT1("DataResidue %x\n", CSW.DataResidue);
    DPRINT1("Status %x\n", CSW.Status);

    //
    // FIXME: handle error
    //
    ASSERT(CSW.Status == 0);
    ASSERT(CSW.DataResidue == 0);

    //
    // calculate transfer length
    //
    *TransferBufferLength = Request->DataTransferLength - CSW.DataResidue;

    //
    // copy buffer
    //
    RtlCopyMemory(Request->DataBuffer, Response, *TransferBufferLength);

    //
    // free item
    //
    FreeItem(OutControl);

    //
    // free response
    //
    FreeItem(Response);

    //
    // done
    //
    return Status;
#endif
}

NTSTATUS
USBSTOR_SendReadCmd(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp)
{
    UFI_READ_CMD Cmd;
    NTSTATUS Status;
    PPDO_DEVICE_EXTENSION PDODeviceExtension;
    PCDB pCDB;
    ULONG BlockCount;
    PIO_STACK_LOCATION IoStack;
    PSCSI_REQUEST_BLOCK Request;

    //
    // get current stack location
    //
    IoStack = IoGetCurrentIrpStackLocation(Irp);

    //
    // get request block
    //
    Request = (PSCSI_REQUEST_BLOCK)IoStack->Parameters.Others.Argument1;

    //
    // get SCSI command data block
    //
    pCDB = (PCDB)Request->Cdb;

    //
    // get PDO device extension
    //
    PDODeviceExtension = (PPDO_DEVICE_EXTENSION)DeviceObject->DeviceExtension;

    //
    // FIXME: support more logical blocks
    //
    DPRINT1("Request->DataTransferLength %x, PDODeviceExtension->BlockLength %x\n", Request->DataTransferLength, PDODeviceExtension->BlockLength);
    ASSERT(Request->DataTransferLength == PDODeviceExtension->BlockLength);

    //
    // block count
    //
    BlockCount = Request->DataTransferLength / PDODeviceExtension->BlockLength;

    //
    // initialize read cmd
    //
    RtlZeroMemory(&Cmd, sizeof(UFI_READ_CMD));
    Cmd.Code = SCSIOP_READ;
    Cmd.LUN = (PDODeviceExtension->LUN & MAX_LUN);
    Cmd.ContiguousLogicBlocks = _byteswap_ushort(BlockCount);

    RtlCopyMemory(&Cmd.LogicalBlockAddress, pCDB->READ12.LogicalBlock, sizeof(UCHAR) * 4);

    DPRINT1("BlockAddress %lu BlockCount %lu BlockLength %lu\n", NTOHL(Cmd.LogicalBlockAddress), BlockCount, PDODeviceExtension->BlockLength);

    //
    // send request
    //
    return USBSTOR_SendRequest(DeviceObject, Irp, NULL, UFI_READ_CMD_LEN, (PUCHAR)&Cmd, Request->DataTransferLength, (PUCHAR)Request->DataBuffer);
}

NTSTATUS
USBSTOR_SendTestUnitCmd(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp)
{
    UFI_TEST_UNIT_CMD Cmd;
    PPDO_DEVICE_EXTENSION PDODeviceExtension;
    PIO_STACK_LOCATION IoStack;
    PSCSI_REQUEST_BLOCK Request;

    //
    // get current stack location
    //
    IoStack = IoGetCurrentIrpStackLocation(Irp);

    //
    // get request block
    //
    Request = (PSCSI_REQUEST_BLOCK)IoStack->Parameters.Others.Argument1;

    //
    // no transfer length
    //
    ASSERT(Request->DataTransferLength == 0);

    //
    // get PDO device extension
    //
    PDODeviceExtension = (PPDO_DEVICE_EXTENSION)DeviceObject->DeviceExtension;

    //
    // initialize test unit cmd
    //
    RtlZeroMemory(&Cmd, sizeof(UFI_TEST_UNIT_CMD));
    Cmd.Code = SCSIOP_TEST_UNIT_READY;
    Cmd.LUN = (PDODeviceExtension->LUN & MAX_LUN);

    //
    // send the request
    //
    return USBSTOR_SendRequest(DeviceObject, Irp, NULL, UFI_TEST_UNIT_CMD_LEN, (PUCHAR)&Cmd, 0, NULL);
}
