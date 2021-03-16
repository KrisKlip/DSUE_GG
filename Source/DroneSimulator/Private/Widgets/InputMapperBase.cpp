// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InputMapperBase.h"

void UInputMapperBase::ListenForAxisEvent(FName axis, FInputAxisDelegate func)
{
    if (InputComponent == nullptr)
    {
        InitializeInputComponent();
    }

    FInputAxisBinding newBinding(axis);
    for (const auto& Binding : InputComponent->AxisBindings)
    {
        if (Binding.AxisName == axis)
        {
            newBinding = Binding;
            newBinding.AxisDelegate.GetDelegateForManualSet().BindUObject(this, &ThisClass::OnAxisValue, func);
            return;
        }
    }
    newBinding.AxisDelegate.GetDelegateForManualSet().BindUObject(this, &ThisClass::OnAxisValue, func);
    InputComponent->AxisBindings.Add(newBinding);
}

void UInputMapperBase::StopListenForAxisEvent(FName axis)
{
    if (InputComponent == nullptr)
    {
        return;
    }

    FInputAxisBinding newBinding(axis);
    for (const auto& Binding : InputComponent->AxisBindings)
    {
        if (Binding.AxisName == axis)
        {
            newBinding = Binding;
            newBinding.AxisDelegate.Unbind();
            return;
        }
    }
}

void UInputMapperBase::OnAxisValue(float value, FInputAxisDelegate func)
{
    func.Execute(value);
}