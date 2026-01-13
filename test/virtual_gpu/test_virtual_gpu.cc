#include <gtest/gtest.h>

#include "virtual_gpu_tester.h"

using namespace ho;

TEST(VirtualGPUTest, InitializeSuccess) { EXPECT_TRUE(VirtualGPUTester::InitializeSuccess()); }
TEST(VirtualGPUTest, InitializeFail) { EXPECT_TRUE(VirtualGPUTester::InitializeFail()); }

TEST(VirtualGPUTest, VaryingOut) { EXPECT_TRUE(VirtualGPUTester::VaryingOut()); }
TEST(VirtualGPUTest, FragmentIn) { EXPECT_TRUE(VirtualGPUTester::FragmentIn()); }
TEST(VirtualGPUTest, FSOutput) { EXPECT_TRUE(VirtualGPUTester::FSOutputOut()); }

TEST(VirtualGPUTest, GetColorLock) { EXPECT_TRUE(VirtualGPUTester::GetColorLock()); }
TEST(VirtualGPUTest, GetDepthLock) { EXPECT_TRUE(VirtualGPUTester::GetDepthLock()); }

TEST(VirtualGPUTest, ClearColorAttachmentFullViewport) {
    EXPECT_TRUE(VirtualGPUTester::ClearColorAttachmentFullViewport());
}
TEST(VirtualGPUTest, ClearColorAttachmentPartialViewport) {
    EXPECT_TRUE(VirtualGPUTester::ClearColorAttachmentPartialViewport());
}
TEST(VirtualGPUTest, ClearColorAttachmentViewportScissorIntersect) {
    EXPECT_TRUE(VirtualGPUTester::ClearColorAttachmentViewportScissorIntersect());
}
TEST(VirtualGPUTest, ClearColorAttachmentColorMask) { EXPECT_TRUE(VirtualGPUTester::ClearColorAttachmentColorMask()); }

TEST(VirtualGPUTest, ClearDepthAttachmentFullViewport) {
    EXPECT_TRUE(VirtualGPUTester::ClearDepthAttachmentFullViewport());
}
TEST(VirtualGPUTest, ClearDepthAttachmentPartialViewport) {
    EXPECT_TRUE(VirtualGPUTester::ClearDepthAttachmentPartialViewport());
}
TEST(VirtualGPUTest, ClearDepthAttachmentViewportScissorIntersect) {
    EXPECT_TRUE(VirtualGPUTester::ClearDepthAttachmentViewportScissorIntersect());
}

TEST(VirtualGPUTest, ClearDepthStencilAttachmentFullViewport) {
    EXPECT_TRUE(VirtualGPUTester::ClearDepthStencilAttachmentFullViewport());
}
TEST(VirtualGPUTest, ClearDepthStencilAttachmentPartialViewport) {
    EXPECT_TRUE(VirtualGPUTester::ClearDepthStencilAttachmentPartialViewport());
}
TEST(VirtualGPUTest, ClearDepthStencilAttachmentViewportScissorIntersect) {
    EXPECT_TRUE(VirtualGPUTester::ClearDepthStencilAttachmentViewportScissorIntersect());
}

TEST(VirtualGPUTest, EvalFrustumPlaneInside) { EXPECT_TRUE(VirtualGPUTester::EvalFrustumPlaneInside()); }
TEST(VirtualGPUTest, EvalFrustumPlaneOutside) { EXPECT_TRUE(VirtualGPUTester::EvalFrustumPlaneOutside()); }
TEST(VirtualGPUTest, EvalFrustumPlaneOnBoundary) { EXPECT_TRUE(VirtualGPUTester::EvalFrustumPlaneOnBoundary()); }
TEST(VirtualGPUTest, IsInsideInside) { EXPECT_TRUE(VirtualGPUTester::IsInsideInside()); }
TEST(VirtualGPUTest, IsInsideOutside) { EXPECT_TRUE(VirtualGPUTester::IsInsideOutside()); }
TEST(VirtualGPUTest, IsInsideOnBoundary) { EXPECT_TRUE(VirtualGPUTester::IsInsideOnBoundary()); }

TEST(VirtualGPUTest, GetClipBarycentricBothInside) { EXPECT_TRUE(VirtualGPUTester::GetClipBarycentricBothInside()); }
TEST(VirtualGPUTest, GetClipBarycentricInsideOutside) {
    EXPECT_TRUE(VirtualGPUTester::GetClipBarycentricInsideOutside());
}
TEST(VirtualGPUTest, GetClipBarycentricInsideOnPlane) {
    EXPECT_TRUE(VirtualGPUTester::GetClipBarycentricInsideOnPlane());
}
TEST(VirtualGPUTest, GetClipBarycentricBothOnPlane) { EXPECT_TRUE(VirtualGPUTester::GetClipBarycentricBothOnPlane()); }
TEST(VirtualGPUTest, GetClipBarycentricBothOutside) { EXPECT_TRUE(VirtualGPUTester::GetClipBarycentricBothOutside()); }

TEST(VirtualGPUTest, LerpVaryingBarycentric10) { EXPECT_TRUE(VirtualGPUTester::LerpVaryingBarycentric10()); }
TEST(VirtualGPUTest, LerpVaryingBarycentric01) { EXPECT_TRUE(VirtualGPUTester::LerpVaryingBarycentric01()); }
TEST(VirtualGPUTest, LerpVaryingBarycentricHalf) { EXPECT_TRUE(VirtualGPUTester::LerpVaryingBarycentricHalf()); }

TEST(VirtualGPUTest, ClipAgainstPlanePointInside) { EXPECT_TRUE(VirtualGPUTester::ClipAgainstPlanePointInside()); }
TEST(VirtualGPUTest, ClipAgainstPlanePointOnPlane) { EXPECT_TRUE(VirtualGPUTester::ClipAgainstPlanePointOnPlane()); }
TEST(VirtualGPUTest, ClipAgainstPlanePointOutside) { EXPECT_TRUE(VirtualGPUTester::ClipAgainstPlanePointOutside()); }

TEST(VirtualGPUTest, ClipAgainstPlaneLineInsideInside) {
    EXPECT_TRUE(VirtualGPUTester::ClipAgainstPlaneLineInsideInside());
}
TEST(VirtualGPUTest, ClipAgainstPlaneLineInsideOutside) {
    EXPECT_TRUE(VirtualGPUTester::ClipAgainstPlaneLineInsideOutside());
}
TEST(VirtualGPUTest, ClipAgainstPlaneLineOutsideInside) {
    EXPECT_TRUE(VirtualGPUTester::ClipAgainstPlaneLineOutsideInside());
}
TEST(VirtualGPUTest, ClipAgainstPlaneLineOutsideOutside) {
    EXPECT_TRUE(VirtualGPUTester::ClipAgainstPlaneLineOutsideOutside());
}
TEST(VirtualGPUTest, ClipAgainstPlaneLineInsideOnPlane) {
    EXPECT_TRUE(VirtualGPUTester::ClipAgainstPlaneLineInsideOnPlane());
}
TEST(VirtualGPUTest, ClipAgainstPlaneLineOnPlaneOnPlane) {
    EXPECT_TRUE(VirtualGPUTester::ClipAgainstPlaneLineOnPlaneOnPlane());
}
TEST(VirtualGPUTest, ClipAgainstPlaneLineIdentical) { EXPECT_TRUE(VirtualGPUTester::ClipAgainstPlaneLineIdentical()); }
TEST(VirtualGPUTest, ClipAgainstPlaneLineOutsideOnPlane) {
    EXPECT_TRUE(VirtualGPUTester::ClipAgainstPlaneLineOutsideOnPlane());
}

TEST(VirtualGPUTest, ClipAgainstPlaneTriangleAllInside) {
    EXPECT_TRUE(VirtualGPUTester::ClipAgainstPlaneTriangleAllInside());
}
TEST(VirtualGPUTest, ClipAgainstPlaneTriangleOneOutside) {
    EXPECT_TRUE(VirtualGPUTester::ClipAgainstPlaneTriangleOneOutside());
}
TEST(VirtualGPUTest, ClipAgainstPlaneTriangleTwoOutside) {
    EXPECT_TRUE(VirtualGPUTester::ClipAgainstPlaneTriangleTwoOutside());
}
TEST(VirtualGPUTest, ClipAgainstPlaneTriangleAllOutside) {
    EXPECT_TRUE(VirtualGPUTester::ClipAgainstPlaneTriangleAllOutside());
}
TEST(VirtualGPUTest, ClipAgainstPlaneTriangleOneOnPlane) {
    EXPECT_TRUE(VirtualGPUTester::ClipAgainstPlaneTriangleOneOnPlane());
}
TEST(VirtualGPUTest, ClipAgainstPlaneTriangleTwoOnPlane) {
    EXPECT_TRUE(VirtualGPUTester::ClipAgainstPlaneTriangleTwoOnPlane());
}
TEST(VirtualGPUTest, ClipAgainstPlaneTriangleTwoIdentical) {
    EXPECT_TRUE(VirtualGPUTester::ClipAgainstPlaneTriangleTwoIdentical());
}
TEST(VirtualGPUTest, ClipAgainstPlaneTriangleAllIdentical) {
    EXPECT_TRUE(VirtualGPUTester::ClipAgainstPlaneTriangleAllIdentical());
}
TEST(VirtualGPUTest, ClipAgainstPlaneTriangleCollinear) {
    EXPECT_TRUE(VirtualGPUTester::ClipAgainstPlaneTriangleCollinear());
}
TEST(VirtualGPUTest, ClipAgainstPlaneTriangleAllOnPlane) {
    EXPECT_TRUE(VirtualGPUTester::ClipAgainstPlaneTriangleAllOnPlane());
}

TEST(VirtualGPUTest, ClipAgainstPlaneInterpolation) { EXPECT_TRUE(VirtualGPUTester::ClipAgainstPlaneInterpolation()); }

TEST(VirtualGPUTest, ClipAllInside) { EXPECT_TRUE(VirtualGPUTester::ClipAllInside()); }
TEST(VirtualGPUTest, ClipOnePlaneClipped) { EXPECT_TRUE(VirtualGPUTester::ClipOnePlaneClipped()); }
TEST(VirtualGPUTest, ClipTwoPlanesClipped) { EXPECT_TRUE(VirtualGPUTester::ClipTwoPlanesClipped()); }
TEST(VirtualGPUTest, ClipAllOutside) { EXPECT_TRUE(VirtualGPUTester::ClipAllOutside()); }
TEST(VirtualGPUTest, ClipWNegative) { EXPECT_TRUE(VirtualGPUTester::ClipWNegative()); }
TEST(VirtualGPUTest, ClipNearPlane) { EXPECT_TRUE(VirtualGPUTester::ClipNearPlane()); }
TEST(VirtualGPUTest, ClipFarPlane) { EXPECT_TRUE(VirtualGPUTester::ClipFarPlane()); }

TEST(VirtualGPUTest, PerspectiveDivideUnitW) { EXPECT_TRUE(VirtualGPUTester::PerspectiveDivideUnitW()); }
TEST(VirtualGPUTest, PerspectiveDivideNonUnitW) { EXPECT_TRUE(VirtualGPUTester::PerspectiveDivideNonUnitW()); }
TEST(VirtualGPUTest, PerspectiveDivideZeroW) { EXPECT_TRUE(VirtualGPUTester::PerspectiveDivideZeroW()); }

TEST(VirtualGPUTest, ViewportTransformCenter) { EXPECT_TRUE(VirtualGPUTester::ViewportTransformCenter()); }
TEST(VirtualGPUTest, ViewportTransformBoundary) { EXPECT_TRUE(VirtualGPUTester::ViewportTransformBoundary()); }
TEST(VirtualGPUTest, ViewportTransformWithOffset) { EXPECT_TRUE(VirtualGPUTester::ViewportTransformWithOffset()); }

TEST(VirtualGPUTest, ScissorTestDisabled) { EXPECT_TRUE(VirtualGPUTester::ScissorTestDisabled()); }
TEST(VirtualGPUTest, ScissorTestOutsideViewport) { EXPECT_TRUE(VirtualGPUTester::ScissorTestOutsideViewport()); }
TEST(VirtualGPUTest, ScissorTestInsideScissor) { EXPECT_TRUE(VirtualGPUTester::ScissorTestInsideScissor()); }
TEST(VirtualGPUTest, ScissorTestInsideViewportOutsideScissor) {
    EXPECT_TRUE(VirtualGPUTester::ScissorTestInsideViewportOutsideScissor());
}
TEST(VirtualGPUTest, ScissorTestBoundary) { EXPECT_TRUE(VirtualGPUTester::ScissorTestBoundary()); }

TEST(VirtualGPUTest, ComputeDepthSlopeFlatTriangle) { EXPECT_TRUE(VirtualGPUTester::ComputeDepthSlopeFlatTriangle()); }
TEST(VirtualGPUTest, ComputeDepthSlopeLinearInX) { EXPECT_TRUE(VirtualGPUTester::ComputeDepthSlopeLinearInX()); }
TEST(VirtualGPUTest, ComputeDepthSlopeLinearInY) { EXPECT_TRUE(VirtualGPUTester::ComputeDepthSlopeLinearInY()); }
TEST(VirtualGPUTest, ComputeDepthSlopeLinearXY) { EXPECT_TRUE(VirtualGPUTester::ComputeDepthSlopeLinearXY()); }
TEST(VirtualGPUTest, ComputeDepthSlopeNegativeGradient) {
    EXPECT_TRUE(VirtualGPUTester::ComputeDepthSlopeNegativeGradient());
}
TEST(VirtualGPUTest, ComputeDepthSlopeDegenerateTriangle) {
    EXPECT_TRUE(VirtualGPUTester::ComputeDepthSlopeDegenerateTriangle());
}

TEST(VirtualGPUTest, ApplyDepthOffsetDisabled) { EXPECT_TRUE(VirtualGPUTester::ApplyDepthOffsetDisabled()); }
TEST(VirtualGPUTest, ApplyDepthOffsetSlopeOnly) { EXPECT_TRUE(VirtualGPUTester::ApplyDepthOffsetSlopeOnly()); }
TEST(VirtualGPUTest, ApplyDepthOffsetUnitOnly24Bit) { EXPECT_TRUE(VirtualGPUTester::ApplyDepthOffsetUnitOnly24Bit()); }
TEST(VirtualGPUTest, ApplyDepthOffsetUnitOnly32Bit) { EXPECT_TRUE(VirtualGPUTester::ApplyDepthOffsetUnitOnly32Bit()); }
TEST(VirtualGPUTest, ApplyDepthOffsetSlopeAndUnit) { EXPECT_TRUE(VirtualGPUTester::ApplyDepthOffsetSlopeAndUnit()); }
TEST(VirtualGPUTest, ApplyDepthOffsetClampUpper) { EXPECT_TRUE(VirtualGPUTester::ApplyDepthOffsetClampUpper()); }
TEST(VirtualGPUTest, ApplyDepthOffsetClampLower) { EXPECT_TRUE(VirtualGPUTester::ApplyDepthOffsetClampLower()); }
TEST(VirtualGPUTest, ApplyDepthOffsetAllPrimitiveModes) {
    EXPECT_TRUE(VirtualGPUTester::ApplyDepthOffsetAllPrimitiveModes());
}
TEST(VirtualGPUTest, TestDepthStencilOutOfRange) { EXPECT_TRUE(VirtualGPUTester::TestDepthStencilOutOfRange()); }
TEST(VirtualGPUTest, TestDepthStencilNoMemory) { EXPECT_TRUE(VirtualGPUTester::TestDepthStencilNoMemory()); }
TEST(VirtualGPUTest, TestDepthStencilDepthTestOffWriteOff_DEPTH_COMPONENT) {
    EXPECT_TRUE(VirtualGPUTester::TestDepthStencilDepthTestOffWriteOff_DepthComponent());
}
TEST(VirtualGPUTest, TestDepthStencilDepthTestOffWriteOn_DEPTH_COMPONENT) {
    EXPECT_TRUE(VirtualGPUTester::TestDepthStencilDepthTestOffWriteOn_DepthComponent());
}
TEST(VirtualGPUTest, TestDepthStencilDepthTestOnWriteOff_DEPTH_COMPONENT) {
    EXPECT_TRUE(VirtualGPUTester::TestDepthStencilDepthTestOnWriteOff_DepthComponent());
}
TEST(VirtualGPUTest, TestDepthStencilDepthTestOnWriteOn_DEPTH_COMPONENT) {
    EXPECT_TRUE(VirtualGPUTester::TestDepthStencilDepthTestOnWriteOn_DepthComponent());
}
TEST(VirtualGPUTest, TestDepthStencilDepthComparison) {
    EXPECT_TRUE(VirtualGPUTester::TestDepthStencilDepthComparison());
}
TEST(VirtualGPUTest, TestDepthStencilDepthOffStencilOff_DEPTH_STENCIL) {
    EXPECT_TRUE(VirtualGPUTester::TestDepthStencilDepthOffStencilOff_DepthStencil());
}
TEST(VirtualGPUTest, TestDepthStencilDepthOnStencilOff_DEPTH_STENCIL) {
    EXPECT_TRUE(VirtualGPUTester::TestDepthStencilDepthOnStencilOff_DepthStencil());
}
TEST(VirtualGPUTest, TestDepthStencilDepthOffStencilOn_DEPTH_STENCILl) {
    EXPECT_TRUE(VirtualGPUTester::TestDepthStencilDepthOffStencilOn_DepthStencil());
}
TEST(VirtualGPUTest, TestDepthStencilDepthPassStencilFail_DEPTH_STENCIL) {
    EXPECT_TRUE(VirtualGPUTester::TestDepthStencilDepthPassStencilFail_DepthStencil());
}
TEST(VirtualGPUTest, TestDepthStencilDepthFailStencilPass_DEPTH_STENCIL) {
    EXPECT_TRUE(VirtualGPUTester::TestDepthStencilDepthFailStencilPass_DepthStencil());
}
TEST(VirtualGPUTest, TestDepthStencilDepthPassStencilPass_DEPTH_STENCIL) {
    EXPECT_TRUE(VirtualGPUTester::TestDepthStencilDepthPassStencilPass_DepthStencil());
}

TEST(VirtualGPUTest, GetBlendFactorZeroOne) { EXPECT_TRUE(VirtualGPUTester::GetBlendFactorZeroOne()); }
TEST(VirtualGPUTest, GetBlendFactorSrcColor) { EXPECT_TRUE(VirtualGPUTester::GetBlendFactorSrcColor()); }
TEST(VirtualGPUTest, GetBlendFactorDstColor) { EXPECT_TRUE(VirtualGPUTester::GetBlendFactorDstColor()); }
TEST(VirtualGPUTest, GetBlendFactorAlphaBased) { EXPECT_TRUE(VirtualGPUTester::GetBlendFactorAlphaBased()); }
TEST(VirtualGPUTest, GetBlendFactorConstantColorBased) {
    EXPECT_TRUE(VirtualGPUTester::GetBlendFactorConstantColorBased());
}
TEST(VirtualGPUTest, GetBlendFactorSrcAlphaSaturate) {
    EXPECT_TRUE(VirtualGPUTester::GetBlendFactorSrcAlphaSaturate());
}

TEST(VirtualGPUTest, ApplyBlendEquationAdd) { EXPECT_TRUE(VirtualGPUTester::ApplyBlendEquationAdd()); }
TEST(VirtualGPUTest, ApplyBlendEquationSubtract) { EXPECT_TRUE(VirtualGPUTester::ApplyBlendEquationSubtract()); }
TEST(VirtualGPUTest, ApplyBlendEquationReverseSubtract) {
    EXPECT_TRUE(VirtualGPUTester::ApplyBlendEquationReverseSubtract());
}
TEST(VirtualGPUTest, ApplyBlendEquationMin) { EXPECT_TRUE(VirtualGPUTester::ApplyBlendEquationMin()); }
TEST(VirtualGPUTest, ApplyBlendEquationMax) { EXPECT_TRUE(VirtualGPUTester::ApplyBlendEquationMax()); }

TEST(VirtualGPUTest, WriteColorNoFrameBuffer) { EXPECT_TRUE(VirtualGPUTester::WriteColorNoFrameBuffer()); }
TEST(VirtualGPUTest, WriteColorDrawSlotDisabled) { EXPECT_TRUE(VirtualGPUTester::WriteColorDrawSlotDisabled()); }
TEST(VirtualGPUTest, WriteColorAttachmentNoMemory) { EXPECT_TRUE(VirtualGPUTester::WriteColorAttachmentNoMemory()); }
TEST(VirtualGPUTest, WriteColorNoBlendNoMask) { EXPECT_TRUE(VirtualGPUTester::WriteColorNoBlendNoMask()); }
TEST(VirtualGPUTest, WriteColorMaskPartial) { EXPECT_TRUE(VirtualGPUTester::WriteColorMaskPartial()); }
TEST(VirtualGPUTest, WriteColorBlendSrcOnly) { EXPECT_TRUE(VirtualGPUTester::WriteColorBlendSrcOnly()); }
TEST(VirtualGPUTest, WriteColorBlendDstOnly) { EXPECT_TRUE(VirtualGPUTester::WriteColorBlendDstOnly()); }

TEST(VirtualGPUTest, RasterizePoint) { EXPECT_TRUE(VirtualGPUTester::RasterizePoint()); }
TEST(VirtualGPUTest, RasterizePointEarlyDepthFailed) {
    EXPECT_TRUE(VirtualGPUTester::RasterizePointEarlyDepthFailed());
}
TEST(VirtualGPUTest, RasterizePointEarlyStencilFailed) {
    EXPECT_TRUE(VirtualGPUTester::RasterizePointEarlyStencilFailed());
}

TEST(VirtualGPUTest, RasterizeLineNonSteep) { EXPECT_TRUE(VirtualGPUTester::RasterizeLineNonSteep()); }
TEST(VirtualGPUTest, RasterizeLineSteep) { EXPECT_TRUE(VirtualGPUTester::RasterizeLineSteep()); }
TEST(VirtualGPUTest, RasterizeLineVertical) { EXPECT_TRUE(VirtualGPUTester::RasterizeLineVertical()); }
TEST(VirtualGPUTest, RasterizeLineHorizontal) { EXPECT_TRUE(VirtualGPUTester::RasterizeLineHorizontal()); }
TEST(VirtualGPUTest, RasterizeLineDegenerated) { EXPECT_TRUE(VirtualGPUTester::RasterizeLineDegenerated()); }
TEST(VirtualGPUTest, RasterizeLineEarlyDepthFailed) { EXPECT_TRUE(VirtualGPUTester::RasterizeLineEarlyDepthFailed()); }
TEST(VirtualGPUTest, RasterizeLineEarlyStencilFailed) {
    EXPECT_TRUE(VirtualGPUTester::RasterizeLineEarlyStencilFailed());
}

TEST(VirtualGPUTest, RasterizeTriangleCCWFront) { EXPECT_TRUE(VirtualGPUTester::RasterizeTriangleCCWFront()); }
TEST(VirtualGPUTest, RasterizeTriangleCCWBack) { EXPECT_TRUE(VirtualGPUTester::RasterizeTriangleCCWBack()); }
TEST(VirtualGPUTest, RasterizeTriangleCCWFrontAndBack) {
    EXPECT_TRUE(VirtualGPUTester::RasterizeTriangleCCWFrontAndBack());
}
TEST(VirtualGPUTest, RasterizeTriangleCWFront) { EXPECT_TRUE(VirtualGPUTester::RasterizeTriangleCWFront()); }
TEST(VirtualGPUTest, RasterizeTriangleCWBack) { EXPECT_TRUE(VirtualGPUTester::RasterizeTriangleCWBack()); }
TEST(VirtualGPUTest, RasterizeTriangleCWFrontAndBack) {
    EXPECT_TRUE(VirtualGPUTester::RasterizeTriangleCWFrontAndBack());
}
TEST(VirtualGPUTest, RasterizeTriangleDegeneratedInPoint) {
    EXPECT_TRUE(VirtualGPUTester::RasterizeTriangleDegeneratedInPoint());
}
TEST(VirtualGPUTest, RasterizeTriangleEarlyDepthFailed) {
    EXPECT_TRUE(VirtualGPUTester::RasterizeTriangleEarlyDepthFailed());
}
TEST(VirtualGPUTest, RasterizeTriangleEarlyStencilFailed) {
    EXPECT_TRUE(VirtualGPUTester::RasterizeTriangleEarlyStencilFailed());
}
