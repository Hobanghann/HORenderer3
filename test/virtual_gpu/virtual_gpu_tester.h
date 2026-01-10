#pragma once

#include "virtual_gpu/virtual_gpu.h"

namespace ho {
    class VirtualGPUTester {
       public:
        static bool InitFreshGPU();

        static bool InitializeSuccess();
        static bool InitializeFail();

        static bool GetColorLock();
        static bool GetDepthLock();

        static bool ClearColorAttachmentFullViewport();
        static bool ClearColorAttachmentPartialViewport();
        static bool ClearColorAttachmentViewportScissorIntersect();
        static bool ClearColorAttachmentColorMask();

        static bool ClearDepthAttachmentFullViewport();
        static bool ClearDepthAttachmentPartialViewport();
        static bool ClearDepthAttachmentViewportScissorIntersect();

        static bool ClearDepthStencilAttachmentFullViewport();
        static bool ClearDepthStencilAttachmentPartialViewport();
        static bool ClearDepthStencilAttachmentViewportScissorIntersect();
        static bool ClearDepthStencilAttachmentDepthOnly();
        static bool ClearDepthStencilAttachmentStencilOnly();
        static bool ClearDepthStencilAttachmentStencilOnlyWithMask();

        static bool EvalFrustumPlaneInside();
        static bool EvalFrustumPlaneOutside();
        static bool EvalFrustumPlaneOnBoundary();

        static bool IsInsideInside();
        static bool IsInsideOutside();
        static bool IsInsideOnBoundary();

        static bool GetClipBarycentricBothInside();
        static bool GetClipBarycentricInsideOutside();
        static bool GetClipBarycentricInsideOnPlane();
        static bool GetClipBarycentricBothOnPlane();
        static bool GetClipBarycentricBothOutside();

        static bool LerpVaryingBarycentric10();
        static bool LerpVaryingBarycentric01();
        static bool LerpVaryingBarycentricHalf();

        static bool ClipAgainstPlanePointInside();
        static bool ClipAgainstPlanePointOnPlane();
        static bool ClipAgainstPlanePointOutside();

        static bool ClipAgainstPlaneLineInsideInside();
        static bool ClipAgainstPlaneLineInsideOutside();
        static bool ClipAgainstPlaneLineOutsideInside();
        static bool ClipAgainstPlaneLineOutsideOutside();
        static bool ClipAgainstPlaneLineInsideOnPlane();
        static bool ClipAgainstPlaneLineOutsideOnPlane();
        static bool ClipAgainstPlaneLineOnPlaneOnPlane();
        static bool ClipAgainstPlaneLineIdentical();

        static bool ClipAgainstPlaneTriangleAllInside();
        static bool ClipAgainstPlaneTriangleOneOutside();
        static bool ClipAgainstPlaneTriangleTwoOutside();
        static bool ClipAgainstPlaneTriangleAllOutside();
        static bool ClipAgainstPlaneTriangleOneOnPlane();
        static bool ClipAgainstPlaneTriangleTwoOnPlane();
        static bool ClipAgainstPlaneTriangleAllOnPlane();
        static bool ClipAgainstPlaneTriangleTwoIdentical();
        static bool ClipAgainstPlaneTriangleAllIdentical();
        static bool ClipAgainstPlaneTriangleCollinear();

        static bool ClipAllInside();
        static bool ClipOnePlaneClipped();
        static bool ClipTwoPlanesClipped();
        static bool ClipAllOutside();
        static bool ClipWNegative();
        static bool ClipNearPlane();
        static bool ClipFarPlane();

        static bool PerspectiveDivideUnitW();
        static bool PerspectiveDivideNonUnitW();
        static bool PerspectiveDivideZeroW();

        static bool ViewportTransformCenter();
        static bool ViewportTransformBoundary();
        static bool ViewportTransformWithOffset();

        static bool ScissorTestDisabled();
        static bool ScissorTestOutsideViewport();
        static bool ScissorTestInsideScissor();
        static bool ScissorTestInsideViewportOutsideScissor();
        static bool ScissorTestBoundary();

        static bool ComputeDepthSlopeFlatTriangle();
        static bool ComputeDepthSlopeLinearInX();
        static bool ComputeDepthSlopeLinearInY();
        static bool ComputeDepthSlopeLinearXY();
        static bool ComputeDepthSlopeNegativeGradient();
        static bool ComputeDepthSlopeDegenerateTriangle();

        static bool ApplyDepthOffsetDisabled();
        static bool ApplyDepthOffsetSlopeOnly();
        static bool ApplyDepthOffsetUnitOnly24Bit();
        static bool ApplyDepthOffsetUnitOnly32Bit();
        static bool ApplyDepthOffsetSlopeAndUnit();
        static bool ApplyDepthOffsetClampUpper();
        static bool ApplyDepthOffsetClampLower();
        static bool ApplyDepthOffsetAllPrimitiveModes();

        static bool TestDepthStencilOutOfRange();
        static bool TestDepthStencilNoMemory();
        static bool TestDepthStencilDepthTestOffWriteOff_DepthComponent();
        static bool TestDepthStencilDepthTestOffWriteOn_DepthComponent();
        static bool TestDepthStencilDepthTestOnWriteOff_DepthComponent();
        static bool TestDepthStencilDepthTestOnWriteOn_DepthComponent();
        static bool TestDepthStencilDepthComparison();
        static bool TestDepthStencilDepthOffStencilOff_DepthStencil();
        static bool TestDepthStencilDepthOnStencilOff_DepthStencil();
        static bool TestDepthStencilDepthOffStencilOn_DepthStencil();
        static bool TestDepthStencilDepthPassStencilFail_DepthStencil();
        static bool TestDepthStencilDepthFailStencilPass_DepthStencil();
        static bool TestDepthStencilDepthPassStencilPass_DepthStencil();

        static bool GetBlendFactorZeroOne();
        static bool GetBlendFactorSrcColor();
        static bool GetBlendFactorDstColor();
        static bool GetBlendFactorAlphaBased();
        static bool GetBlendFactorConstantColorBased();
        static bool GetBlendFactorSrcAlphaSaturate();

        static bool ApplyBlendEquationAdd();
        static bool ApplyBlendEquationSubtract();
        static bool ApplyBlendEquationReverseSubtract();
        static bool ApplyBlendEquationMin();
        static bool ApplyBlendEquationMax();

        static bool WriteColorNoFrameBuffer();
        static bool WriteColorDrawSlotDisabled();
        static bool WriteColorAttachmentNoMemory();
        static bool WriteColorNoBlendNoMask();
        static bool WriteColorMaskPartial();
        static bool WriteColorBlendSrcOnly();
        static bool WriteColorBlendDstOnly();

        static bool RasterizePoint();
        static bool RasterizePointEarlyDepthFailed();
        static bool RasterizePointEarlyStencilFailed();

        static bool RasterizeLineNonSteep();
        static bool RasterizeLineSteep();
        static bool RasterizeLineVertical();
        static bool RasterizeLineHorizontal();
        static bool RasterizeLineDegenerated();
        static bool RasterizeLineEarlyDepthFailed();
        static bool RasterizeLineEarlyStencilFailed();

        static bool RasterizeTriangleCCWFront();
        static bool RasterizeTriangleCCWBack();
        static bool RasterizeTriangleCCWFrontAndBack();
        static bool RasterizeTriangleCWFront();
        static bool RasterizeTriangleCWBack();
        static bool RasterizeTriangleCWFrontAndBack();
        static bool RasterizeTriangleDegeneratedInPoint();
        static bool RasterizeTriangleEarlyDepthFailed();
        static bool RasterizeTriangleEarlyStencilFailed();

       private:
        static VirtualGPU::Varying MakeVarying(float base) {
            VirtualGPU::Varying v;
            v.clip_coord = Vector4(base + 1.0_r, base + 2.0_r, base + 3.0_r, base + 4.0_r);
            v.world_pos = Vector3(base + 5.0_r, base + 6.0_r, base + 7.0_r);
            v.view_pos = Vector3(base + 8.0_r, base + 9.0_r, base + 10.0_r);
            v.normal = Vector3(base + 11.0_r, base + 12.0_r, base + 13.0_r);
            v.tangent = Vector4(base + 14.0_r, base + 15.0_r, base + 16.0_r, 1.0_r);
            v.uv0 = Vector2(base + 18.0_r, base + 19.0_r);
            v.uv1 = Vector2(base + 20.0_r, base + 21.0_r);
            v.color0 = Color128(10.f + base, 20.f, 30.f, 40.f);
            v.color1 = Color128(50.f, 60.f + base, 70.f, 80.f);
            return v;
        }
    };
}  // namespace ho
