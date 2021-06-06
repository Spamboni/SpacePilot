﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Containers/StaticArray.h"

#if WITH_3DX_NAVLIB

#include "CoreMinimal.h"

#include <navlib/navlib_types.h>
#include <navlib/navlib.h>


#define DECLARE_TO_UNREAL_BODY(...) __VA_ARGS__
#define DECLARE_TO_UNREAL(MNavlib, MUnreal, FromBody, ToBody, SetValBody, GetValBody) \
    template<> \
    struct TUnreal<MNavlib> \
    { \
        typedef MUnreal FUnreal; \
        typedef MNavlib FNavlib; \
        static FUnreal From(const FNavlib& $) { return MoveTemp<FUnreal>(DECLARE_TO_UNREAL_BODY##FromBody); } \
        static FNavlib To(const FUnreal& $) { return MoveTemp<FNavlib>(DECLARE_TO_UNREAL_BODY##ToBody); } \
    }

namespace navlib
{
    template<typename TNavlibType>
    struct TUnreal {};

    DECLARE_TO_UNREAL(
        point_t, FVector,
        ({$.x, $.y, $.z}),
        ({$.X, $.Y, $.Z})
    );

    DECLARE_TO_UNREAL(
        vector_t, FVector,
        ({$.x, $.y, $.z}),
        ({$.X, $.Y, $.Z})
    );

    DECLARE_TO_UNREAL(
        plane_t, FPlane,
        ({$.x, $.y, $.z, $.d}),
        ({$.X, $.Y, $.Z, $.W})
    );

    DECLARE_TO_UNREAL(
        box_t, FBox,
        ({TUnreal<point_t>::From($.min), TUnreal<point_t>::From($.max)}),
        ({$.Min.X, $.Min.Y, $.Min.Z, $.Max.X, $.Max.Y, $.Max.Z})
    );

    DECLARE_TO_UNREAL(
        frustum_t, FMatrix,
        ({
            {
                -($.farVal + $.nearVal) / ($.farVal-$.nearVal),
                0.0f,
                0.0f,
                -2.0f * $.nearVal * $.farVal / ($.farVal-$.nearVal)
            },
            {
                ($.right+$.left) / ($.right - $.left),
                2.0f * $.nearVal / ($.right - $.left),
                0.0f,
                0.0f
            },
            {
                ($.top + $.bottom) / ($.top - $.bottom),
                0.0f,
                2.0f * $.nearVal / ($.top - $.bottom),
                0.0f
            },
            {1.0f, 0.0f, 0.0f, 0.0f}
        }),
        ([&](){
            FPlane L, R, T, B, N, F;
            if(!$.GetFrustumLeftPlane(L)) return {};
            if(!$.GetFrustumRightPlane(R)) return {};
            if(!$.GetFrustumTopPlane(T)) return {};
            if(!$.GetFrustumBottomPlane(B)) return {};
            if(!$.GetFrustumNearPlane(N)) return {};
            if(!$.GetFrustumFarPlane(F)) return {};

            // TODO: I'm 85% sure this is not correct here:
            return { L.W, R.W, B.W, T.W, N.W, F.W };
        }())
    );

    DECLARE_TO_UNREAL(
        matrix_t, FMatrix,
        ({
            {$.m00, $.m01, $.m02, $.m03},
            {$.m10, $.m11, $.m12, $.m13},
            {$.m20, $.m21, $.m22, $.m23},
            {$.m30, $.m31, $.m32, $.m33}
        }),
        ({
            $.M[0][0], $.M[0][1], $.M[0][2], $.M[0][3],
            $.M[1][0], $.M[1][1], $.M[1][2], $.M[1][3],
            $.M[2][0], $.M[2][1], $.M[2][2], $.M[2][3],
            $.M[3][0], $.M[3][1], $.M[3][2], $.M[3][3]
        })
    );

    DECLARE_TO_UNREAL(
        string_t, FString,
        (FString($.length, ANSI_TO_TCHAR($.p))),
        ({TCHAR_TO_ANSI(*$), $.Len()})
    );

    DECLARE_TO_UNREAL(
        cstr_t, FString,
        (FString($.length, ANSI_TO_TCHAR($.p))),
        ({TCHAR_TO_ANSI(*$), $.Len()})
    );
    
    enum class EProperty : uint8
    {
        Active,
        Focus,
        Motion,
        CoordinateSystem,
        DevicePresent,
        EventsKeyPress,
        EventsKeyRelease,
        Transaction,
        FrameTime,
        FrameTimingSource,
        ViewAffine,
        ViewConstructionPlane,
        ViewExtents,
        ViewFov,
        ViewFrustum,
        ViewPerspective,
        ViewRotatable,
        ViewTarget,
        ViewsFront,
        PivotPosition,
        PivotUser,
        PivotVisible,
        HitLookfrom,
        HitDirection,
        HitAperture,
        HitLookat,
        HitSelectionOnly,
        SelectionAffine,
        SelectionEmpty,
        SelectionExtents,
        ModelExtents,
        PointerPosition,
        CommandsTree,
        CommandsActiveSet,
        CommandsActiveCommand,
        Images,
        Settings,
        SettingsChanged
    };

    inline TArray GPropertyMap {
        active_k,
        focus_k,
        motion_k,
        coordinate_system_k,
        device_present_k,
        events_keyPress_k,
        events_keyRelease_k,
        transaction_k,
        frame_time_k,
        frame_timing_source_k,
        view_affine_k,
        view_constructionPlane_k,
        view_extents_k,
        view_fov_k,
        view_frustum_k,
        view_perspective_k,
        view_rotatable_k,
        view_target_k,
        views_front_k,
        pivot_position_k,
        pivot_user_k,
        pivot_visible_k,
        hit_lookfrom_k,
        hit_direction_k,
        hit_aperture_k,
        hit_lookat_k,
        hit_selectionOnly_k,
        selection_affine_k,
        selection_empty_k,
        selection_extents_k,
        model_extents_k,
        pointer_position_k,
        commands_tree_k,
        commands_activeSet_k,
        commands_activeCommand_k,
        images_k,
        settings_k,
        settings_changed_k
    };

    template<EProperty TProp>
    struct TPropType {};
    
    template<> struct TPropType<EProperty::Active>                { using FType = bool; };
    template<> struct TPropType<EProperty::Focus>                 { using FType = bool; };
    template<> struct TPropType<EProperty::Motion>                { using FType = bool; };
    template<> struct TPropType<EProperty::CoordinateSystem>      { using FType = matrix; };
    template<> struct TPropType<EProperty::DevicePresent>         { using FType = bool; };
    template<> struct TPropType<EProperty::EventsKeyPress>        { using FType = long; };
    template<> struct TPropType<EProperty::EventsKeyRelease>      { using FType = long; };
    template<> struct TPropType<EProperty::Transaction>           { using FType = long; };
    template<> struct TPropType<EProperty::FrameTime>             { using FType = double; };
    template<> struct TPropType<EProperty::FrameTimingSource>     { using FType = long; };
    template<> struct TPropType<EProperty::ViewAffine>            { using FType = matrix; };
    template<> struct TPropType<EProperty::ViewConstructionPlane> { using FType = plane; };
    template<> struct TPropType<EProperty::ViewExtents>           { using FType = box; };
    template<> struct TPropType<EProperty::ViewFov>               { using FType = float; };
    template<> struct TPropType<EProperty::ViewFrustum>           { using FType = frustum; };
    template<> struct TPropType<EProperty::ViewPerspective>       { using FType = bool; };
    template<> struct TPropType<EProperty::ViewRotatable>         { using FType = bool; };
    template<> struct TPropType<EProperty::ViewTarget>            { using FType = point; };
    template<> struct TPropType<EProperty::ViewsFront>            { using FType = matrix; };
    template<> struct TPropType<EProperty::PivotPosition>         { using FType = point; };
    template<> struct TPropType<EProperty::PivotUser>             { using FType = bool; };
    template<> struct TPropType<EProperty::PivotVisible>          { using FType = bool; };
    template<> struct TPropType<EProperty::HitLookfrom>           { using FType = point; };
    template<> struct TPropType<EProperty::HitDirection>          { using FType = vector; };
    template<> struct TPropType<EProperty::HitAperture>           { using FType = float; };
    template<> struct TPropType<EProperty::HitLookat>             { using FType = point; };
    template<> struct TPropType<EProperty::HitSelectionOnly>      { using FType = bool; };
    template<> struct TPropType<EProperty::SelectionAffine>       { using FType = matrix; };
    template<> struct TPropType<EProperty::SelectionEmpty>        { using FType = bool; };
    template<> struct TPropType<EProperty::SelectionExtents>      { using FType = box; };
    template<> struct TPropType<EProperty::ModelExtents>          { using FType = box; };
    template<> struct TPropType<EProperty::PointerPosition>       { using FType = point; };
    template<> struct TPropType<EProperty::CommandsTree>          { using FType = SiActionNodeEx_t*; };
    template<> struct TPropType<EProperty::CommandsActiveSet>     { using FType = string_t; };
    template<> struct TPropType<EProperty::CommandsActiveCommand> { using FType = string_t; };
    template<> struct TPropType<EProperty::Images>                { using FType = imagearray_t; };
    template<> struct TPropType<EProperty::Settings>              { using FType = string_t; };
    template<> struct TPropType<EProperty::SettingsChanged>       { using FType = long; };

    template<EProperty TProp>
    class TProperty
    {
    public:
        using FSelf = TProperty<TProp>;
        using FTypeNl = typename TPropType<TProp>::FType;
        using FConversion = TUnreal<FTypeNl>;
        using FTypeUe = typename FConversion::FUnreal;
        
        DECLARE_DELEGATE_TwoParams(FChange, FTypeUe, FTypeNl);
        DECLARE_DELEGATE_RetVal(bool, FAvailable);

        static FTypeNl FromUe(const FTypeUe& $) { return MoveTemp(FConversion::To($)); }
        static FTypeUe FromNl(const FTypeNl& $) { return MoveTemp(FConversion::From($)); }
        
        static property_t GetProperty()
        {
            return GPropertyMap[static_cast<uint8>(TProp)];
        }

        explicit TProperty(
            const nlHandle_t& InCtx,
            FChange InOnSet,
            FAvailable InIsAvailable
        ) : Ctx(InCtx)
          , OnSet(InOnSet)
          , IsAvailableDel(InIsAvailable)
        {}
        
        FTypeNl GetCached() { return Cache; }
        void SetCached(const FTypeNl& InVal) { Cache = InVal; }

        FTypeNl Get()
        {
            value_t Val;
            if(NlReadValue(Ctx, GetProperty(), &Val))
            {
                return Cache;
            }

            FTypeNl TypedVal = Val;
            return Val;
        }

        void Set(const FTypeNl& InVal)
        {
            SetCached(InVal);
            
            value_t Val(InVal);
            auto Result = NlWriteValue(Ctx, GetProperty(), &Val);

            check(!Result);
        }

        bool IsAvailable() const
        {
            return IsAvailableDel.Execute();
        }

        operator FTypeUe& () { return FromNl(Get()); }
        operator const FTypeUe& () { return FromNl(Get()); }
        operator FTypeNl& () { return Get(); }
        operator const FTypeNl& () { return Get(); }

        FSelf& operator=(value_t* InVal)
        {
            Set(*InVal);
            return *this;
        }
        FSelf& operator=(const value_t* InVal)
        {
            Set(*InVal);
            return *this;
        }

        FSelf& operator=(const FTypeUe& InVal)
        {
            Set(FromUe(InVal));
            return *this;
        }
        FSelf& operator=(const FTypeNl& InVal)
        {
            Set(InVal);
            return *this;
        }
    
    private:
        nlHandle_t Ctx;
        FTypeNl Cache {};
        FChange OnSet;
        FAvailable IsAvailableDel;
    };
}

#endif