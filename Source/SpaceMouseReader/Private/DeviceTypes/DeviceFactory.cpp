﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "DeviceTypes/DeviceFactory.h"

#include "hidapi.h"
#include "DeviceTypes/ButtonCapabilities.h"
#include "DeviceTypes/SmDevice.h"
#include "DeviceTypes/SpaceMouseEnterpriseLike.h"
#include "DeviceTypes/SpaceMouseProLike.h"
#include "DeviceTypes/SpaceNavigatorLike.h"
#include "DeviceTypes/SpacePilotLike.h"
#include "ReadingMethod/DataReadingMethod.h"
#include "ReadingMethod/SeparateReportTransRotMethod.h"
#include "ReadingMethod/SingleReportTransRotMethod.h"

namespace SmDevFactory_Details
{
    template<typename TButtonCapabilities = FButtonCapabilities, typename TDataReadingMethod = FSeparateReportTransRotMethod>
    FSmDeviceCreator DeclareModel(FString DeviceName, ESmModelConfidence Confidence = ESmModelConfidence::Tested)
    {
        static_assert(std::is_convertible_v<TButtonCapabilities*, FButtonCapabilities*>, "Invalid button assignment class");
        static_assert(std::is_convertible_v<TDataReadingMethod*, FDataReadingMethod*>, "Invalid data reading method class");
        
        return [=](const FSmDeviceInstantiation& InstInfo)
        {
            return MakeShared<FSmDevice>(
                DeviceName,
                Confidence,
                MakeShared<TButtonCapabilities>(),
                MakeShared<TDataReadingMethod>(),
                InstInfo
            );
        };
    }
}

#define DETAILS SmDevFactory_Details

FDeviceFactory::FDeviceFactory()
{
    
    // https://github.com/FreeSpacenav/spacenavd/blob/a9eccf34e7cac969ee399f625aef827f4f4aaec6/src/dev.c#L202
    // https://www.3dconnexion.fr/nc/service/faqs/faq/how-can-i-check-if-my-usb-3d-mouse-is-recognized-by-windows.html
    
    AddModel(0x046d, 0xc603, DETAILS::DeclareModel
    (
        TEXT("Space Mouse plus XT"),
        ESmModelConfidence::Unknown
    ));
    
    AddModel(0x046d, 0xc605, DETAILS::DeclareModel
    (
        TEXT("CAD Man"),
        ESmModelConfidence::Unknown
    ));
    
    AddModel(0x046d, 0xc606, DETAILS::DeclareModel
    (
        TEXT("Space Mouse classic"),
        ESmModelConfidence::Unknown
    ));
    
    AddModel(0x046d, 0xc621, DETAILS::DeclareModel
    (
        TEXT("SpaceBall 5000"),
        ESmModelConfidence::Unknown
    ));
    
    AddModel(0x046d, 0xc623, DETAILS::DeclareModel
    <FSpaceNavigatorLike>
    (
        TEXT("Space Traveler"),
        ESmModelConfidence::UntestedShouldWork
    ));
    
    AddModel(0x046d, 0xc625, DETAILS::DeclareModel
    <FSpacePilotLike>
    (
        TEXT("Space Pilot"),
        ESmModelConfidence::TestedViaFeedback
    ));
    
    AddModel(0x046d, 0xc626, DETAILS::DeclareModel
    <FSpaceNavigatorLike>
    (
        TEXT("Space Navigator")
    ));
    
    AddModel(0x046d, 0xc627, DETAILS::DeclareModel
    <FSpaceMouseProLike>
    (
        TEXT("Space Explorer"),
        ESmModelConfidence::UntestedShouldWork
    ));
    
    AddModel(0x046d, 0xc628, DETAILS::DeclareModel
    <FSpaceNavigatorLike>
    (
        TEXT("Space Navigator for Notebooks"),
        ESmModelConfidence::UntestedShouldWork
    ));
    
    AddModel(0x046d, 0xc629, DETAILS::DeclareModel
    <FSpacePilotLike>
    (
        TEXT("Space Pilot Pro"),
        ESmModelConfidence::TestedViaFeedback
    ));
    
    AddModel(0x046d, 0xc62b, DETAILS::DeclareModel
    <FSpaceMouseProLike>
    (
        TEXT("Space Mouse Pro")
    ));
    
    AddModel(0x046d, 0xc640, DETAILS::DeclareModel
    (
        TEXT("nulooq"),
        ESmModelConfidence::Unsupported
    ));
    
    AddModel(0x256f, 0xc62e, DETAILS::DeclareModel
    <FSpaceNavigatorLike, FSingleReportTransRotMethod>
    (
        TEXT("Space Mouse Wireless (USB cable)")
    ));
    
    AddModel(0x256f, 0xc62f, DETAILS::DeclareModel
    <FSpaceNavigatorLike, FSingleReportTransRotMethod>
    (
        TEXT("Space Mouse Wireless (Receiver)")
    ));
    
    AddModel(0x256f, 0xc631, DETAILS::DeclareModel
    <FSpaceMouseProLike, FSingleReportTransRotMethod>
    (
        TEXT("Space Mouse Pro Wireless (USB cable)")
    ));
    
    AddModel(0x256f, 0xc632, DETAILS::DeclareModel
    <FSpaceMouseProLike, FSingleReportTransRotMethod>
    (
        TEXT("Space Mouse Pro Wireless (Receiver)")
    ));
    
    AddModel(0x256f, 0xc633, DETAILS::DeclareModel
    <FSpaceMouseEnterpriseLike, FSingleReportTransRotMethod>
    (
        TEXT("Space Mouse Enterprise"),
        ESmModelConfidence::TestedViaFeedback
    ));
    
    AddModel(0x256f, 0xc635, DETAILS::DeclareModel
    <FSpaceNavigatorLike>
    (
        TEXT("Space Mouse Compact")
    ));
    
    AddModel(0x256f, 0xc636, DETAILS::DeclareModel
    (
        TEXT("Space Mouse Module"),
        ESmModelConfidence::UntestedShouldWork
    ));
    
    AddModel(0x256f, 0xc652, DETAILS::DeclareModel
    (
        TEXT("Universal Receiver"),
        ESmModelConfidence::Unknown
    ));
}

FDeviceFactory::~FDeviceFactory()
{
}

void FDeviceFactory::AddModel(uint16 Vid, uint16 Pid, FSmDeviceCreator Creator)
{
    KnownModels.Add(JOIN_VIDPID(Vid, Pid), Creator);
}

void FDeviceFactory::OpenConnectedDevices(const TFunction<FUserSettings()>& Settings, TArray<TSharedPtr<FSmDevice>>& Output)
{
    auto cinfo = hid_enumerate(0, 0);
    
    int ii = 0;
    while (cinfo)
    {
        uint32 vidpid = JOIN_VIDPID(
            static_cast<uint32>(cinfo->vendor_id),
            static_cast<uint32>(cinfo->product_id)
        );
        if(KnownModels.Contains(vidpid))
        {
            auto HidDevice = MakeShared<FActiveHidSmDevice>(cinfo);
            if(HidDevice->bDeviceOpened)
            {
                auto Device = KnownModels[vidpid]({ii, HidDevice, Settings});
                Output.Add(Device);
            }
        }
        cinfo = cinfo->next;
        ii++;
    }
}

#undef DETAILS
