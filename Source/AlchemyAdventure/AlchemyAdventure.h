// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(CountessLog, Log, All);

#define print(Text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT(Text), false)
#define print_k(Key, Text) if (GEngine) GEngine->AddOnScreenDebugMessage(Key, 2.0f, FColor::White, TEXT(Text))
#define printf(Format, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, FString::Printf(TEXT(Format), ##__VA_ARGS__) false)
#define printf_k(Key, Format, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(Key, 2.0f, FColor::White, FString::Printf(TEXT(Format), ##_VA_ARGS__))
#define warn(Text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT(Text), false)
#define warn_k(key, Text) if (GEngine) GEngine->AddOnScreenDebugMessage(Key, 2.0f, FColor::Yellow, TEXT(Text))
#define warnf(Format, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT(Format), ##_VA_ARGS__), false)
#define warnf_k(Key, Format, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(Key, 2.0f, FColor::Yellow, FString::Printf(TEXT(Format), ##_VA_ARGS__))