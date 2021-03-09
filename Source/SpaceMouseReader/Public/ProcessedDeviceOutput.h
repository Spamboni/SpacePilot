﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Buttons.h"
#include "Containers/StaticBitArray.h"

/**
 * 
 */
struct SPACEMOUSEREADER_API FProcessedDeviceOutput
{
    FVector Translation {0,0,0};
    FRotator Rotation {0,0,0};
    FButtonBits Buttons;

    FProcessedDeviceOutput operator+(const FProcessedDeviceOutput& other) const;
    FProcessedDeviceOutput operator+(TSharedPtr<FProcessedDeviceOutput> other) const;
    FProcessedDeviceOutput&& operator+=(const FProcessedDeviceOutput& other);
    FProcessedDeviceOutput&& operator+=(TSharedPtr<FProcessedDeviceOutput> other);
};
