#pragma warning( disable : 4061 4324 ) 

//*********************************************************
// NOLINTNEXTLINE
//
// NOLINTNEXTLINE
// Copyright (c) Microsoft. All rights reserved.
// NOLINTNEXTLINE
// This code is licensed under the MIT License (MIT).
// NOLINTNEXTLINE
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// NOLINTNEXTLINE
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// NOLINTNEXTLINE
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// NOLINTNEXTLINE
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
// NOLINTNEXTLINE
//
// NOLINTNEXTLINE
//*********************************************************
// NOLINTNEXTLINE

// NOLINTNEXTLINE
#ifndef __D3DX12_H__
// NOLINTNEXTLINE
#define __D3DX12_H__
// NOLINTNEXTLINE

// NOLINTNEXTLINE
#include "d3d12.h"
// NOLINTNEXTLINE

// NOLINTNEXTLINE
#if defined( __cplusplus )
// NOLINTNEXTLINE

// NOLINTNEXTLINE
struct CD3DX12_DEFAULT {};
// NOLINTNEXTLINE
extern const DECLSPEC_SELECTANY CD3DX12_DEFAULT D3D12_DEFAULT;
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
inline bool operator==( const D3D12_VIEWPORT& l, const D3D12_VIEWPORT& r )
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  return l.TopLeftX == r.TopLeftX && l.TopLeftY == r.TopLeftY && l.Width == r.Width &&
  // NOLINTNEXTLINE
    l.Height == r.Height && l.MinDepth == r.MinDepth && l.MaxDepth == r.MaxDepth;
    // NOLINTNEXTLINE
}
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
inline bool operator!=( const D3D12_VIEWPORT& l, const D3D12_VIEWPORT& r )
// NOLINTNEXTLINE
{ return !( l == r ); }
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_RECT : public D3D12_RECT
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_RECT()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_RECT( const D3D12_RECT& o ) :
  // NOLINTNEXTLINE
    D3D12_RECT( o )
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_RECT(
    // NOLINTNEXTLINE
    LONG Left,
    // NOLINTNEXTLINE
    LONG Top,
    // NOLINTNEXTLINE
    LONG Right,
    // NOLINTNEXTLINE
    LONG Bottom )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    left = Left;
    // NOLINTNEXTLINE
    top = Top;
    // NOLINTNEXTLINE
    right = Right;
    // NOLINTNEXTLINE
    bottom = Bottom;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  ~CD3DX12_RECT() {}
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_VIEWPORT : public D3D12_VIEWPORT
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_VIEWPORT()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_VIEWPORT( const D3D12_VIEWPORT& o ) :
  // NOLINTNEXTLINE
    D3D12_VIEWPORT( o )
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_VIEWPORT(
    // NOLINTNEXTLINE
    FLOAT topLeftX,
    // NOLINTNEXTLINE
    FLOAT topLeftY,
    // NOLINTNEXTLINE
    FLOAT width,
    // NOLINTNEXTLINE
    FLOAT height,
    // NOLINTNEXTLINE
    FLOAT minDepth = D3D12_MIN_DEPTH,
    // NOLINTNEXTLINE
    FLOAT maxDepth = D3D12_MAX_DEPTH )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    TopLeftX = topLeftX;
    // NOLINTNEXTLINE
    TopLeftY = topLeftY;
    // NOLINTNEXTLINE
    Width = width;
    // NOLINTNEXTLINE
    Height = height;
    // NOLINTNEXTLINE
    MinDepth = minDepth;
    // NOLINTNEXTLINE
    MaxDepth = maxDepth;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  explicit CD3DX12_VIEWPORT(
    // NOLINTNEXTLINE
    _In_ ID3D12Resource* pResource,
    // NOLINTNEXTLINE
    UINT mipSlice = 0,
    // NOLINTNEXTLINE
    FLOAT topLeftX = 0.0f,
    // NOLINTNEXTLINE
    FLOAT topLeftY = 0.0f,
    // NOLINTNEXTLINE
    FLOAT minDepth = D3D12_MIN_DEPTH,
    // NOLINTNEXTLINE
    FLOAT maxDepth = D3D12_MAX_DEPTH )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    D3D12_RESOURCE_DESC Desc = pResource->GetDesc();
    // NOLINTNEXTLINE
    const UINT64 SubresourceWidth = Desc.Width >> mipSlice;
    // NOLINTNEXTLINE
    const UINT64 SubresourceHeight = Desc.Height >> mipSlice;
    // NOLINTNEXTLINE
    switch (Desc.Dimension)
    // NOLINTNEXTLINE
    {
      // NOLINTNEXTLINE
    case D3D12_RESOURCE_DIMENSION_BUFFER:
    // NOLINTNEXTLINE
      TopLeftX = topLeftX;
      // NOLINTNEXTLINE
      TopLeftY = 0.0f;
      // NOLINTNEXTLINE
      Width = Desc.Width - topLeftX;
      // NOLINTNEXTLINE
      Height = 1.0f;
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
    // NOLINTNEXTLINE
      TopLeftX = topLeftX;
      // NOLINTNEXTLINE
      TopLeftY = 0.0f;
      // NOLINTNEXTLINE
      Width = (SubresourceWidth ? SubresourceWidth : 1.0f) - topLeftX;
      // NOLINTNEXTLINE
      Height = 1.0f;
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
    // NOLINTNEXTLINE
    case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
    // NOLINTNEXTLINE
      TopLeftX = topLeftX;
      // NOLINTNEXTLINE
      TopLeftY = topLeftY;
      // NOLINTNEXTLINE
      Width = (SubresourceWidth ? SubresourceWidth : 1.0f) - topLeftX;
      // NOLINTNEXTLINE
      Height = (SubresourceHeight ? SubresourceHeight: 1.0f) - topLeftY;
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    default: break;
    // NOLINTNEXTLINE
    }
    // NOLINTNEXTLINE

// NOLINTNEXTLINE
    MinDepth = minDepth;
    // NOLINTNEXTLINE
    MaxDepth = maxDepth;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  ~CD3DX12_VIEWPORT() {}
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_BOX : public D3D12_BOX
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_BOX()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_BOX( const D3D12_BOX& o ) :
  // NOLINTNEXTLINE
    D3D12_BOX( o )
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_BOX(
    // NOLINTNEXTLINE
    LONG Left,
    // NOLINTNEXTLINE
    LONG Right )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    left = Left;
    // NOLINTNEXTLINE
    top = 0;
    // NOLINTNEXTLINE
    front = 0;
    // NOLINTNEXTLINE
    right = Right;
    // NOLINTNEXTLINE
    bottom = 1;
    // NOLINTNEXTLINE
    back = 1;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  explicit CD3DX12_BOX(
    // NOLINTNEXTLINE
    LONG Left,
    // NOLINTNEXTLINE
    LONG Top,
    // NOLINTNEXTLINE
    LONG Right,
    // NOLINTNEXTLINE
    LONG Bottom )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    left = Left;
    // NOLINTNEXTLINE
    top = Top;
    // NOLINTNEXTLINE
    front = 0;
    // NOLINTNEXTLINE
    right = Right;
    // NOLINTNEXTLINE
    bottom = Bottom;
    // NOLINTNEXTLINE
    back = 1;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  explicit CD3DX12_BOX(
    // NOLINTNEXTLINE
    LONG Left,
    // NOLINTNEXTLINE
    LONG Top,
    // NOLINTNEXTLINE
    LONG Front,
    // NOLINTNEXTLINE
    LONG Right,
    // NOLINTNEXTLINE
    LONG Bottom,
    // NOLINTNEXTLINE
    LONG Back )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    left = Left;
    // NOLINTNEXTLINE
    top = Top;
    // NOLINTNEXTLINE
    front = Front;
    // NOLINTNEXTLINE
    right = Right;
    // NOLINTNEXTLINE
    bottom = Bottom;
    // NOLINTNEXTLINE
    back = Back;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  ~CD3DX12_BOX() {}
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE
inline bool operator==( const D3D12_BOX& l, const D3D12_BOX& r )
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  return l.left == r.left && l.top == r.top && l.front == r.front &&
  // NOLINTNEXTLINE
    l.right == r.right && l.bottom == r.bottom && l.back == r.back;
    // NOLINTNEXTLINE
}
// NOLINTNEXTLINE
inline bool operator!=( const D3D12_BOX& l, const D3D12_BOX& r )
// NOLINTNEXTLINE
{ return !( l == r ); }
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_DEPTH_STENCIL_DESC : public D3D12_DEPTH_STENCIL_DESC
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_DEPTH_STENCIL_DESC()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_DEPTH_STENCIL_DESC( const D3D12_DEPTH_STENCIL_DESC& o ) :
  // NOLINTNEXTLINE
    D3D12_DEPTH_STENCIL_DESC( o )
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_DEPTH_STENCIL_DESC( CD3DX12_DEFAULT )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    DepthEnable = TRUE;
    // NOLINTNEXTLINE
    DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    // NOLINTNEXTLINE
    DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    // NOLINTNEXTLINE
    StencilEnable = FALSE;
    // NOLINTNEXTLINE
    StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    // NOLINTNEXTLINE
    StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    // NOLINTNEXTLINE
    const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
    // NOLINTNEXTLINE
    { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
    // NOLINTNEXTLINE
    FrontFace = defaultStencilOp;
    // NOLINTNEXTLINE
    BackFace = defaultStencilOp;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  explicit CD3DX12_DEPTH_STENCIL_DESC(
    // NOLINTNEXTLINE
    BOOL depthEnable,
    // NOLINTNEXTLINE
    D3D12_DEPTH_WRITE_MASK depthWriteMask,
    // NOLINTNEXTLINE
    D3D12_COMPARISON_FUNC depthFunc,
    // NOLINTNEXTLINE
    BOOL stencilEnable,
    // NOLINTNEXTLINE
    UINT8 stencilReadMask,
    // NOLINTNEXTLINE
    UINT8 stencilWriteMask,
    // NOLINTNEXTLINE
    D3D12_STENCIL_OP frontStencilFailOp,
    // NOLINTNEXTLINE
    D3D12_STENCIL_OP frontStencilDepthFailOp,
    // NOLINTNEXTLINE
    D3D12_STENCIL_OP frontStencilPassOp,
    // NOLINTNEXTLINE
    D3D12_COMPARISON_FUNC frontStencilFunc,
    // NOLINTNEXTLINE
    D3D12_STENCIL_OP backStencilFailOp,
    // NOLINTNEXTLINE
    D3D12_STENCIL_OP backStencilDepthFailOp,
    // NOLINTNEXTLINE
    D3D12_STENCIL_OP backStencilPassOp,
    // NOLINTNEXTLINE
    D3D12_COMPARISON_FUNC backStencilFunc )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    DepthEnable = depthEnable;
    // NOLINTNEXTLINE
    DepthWriteMask = depthWriteMask;
    // NOLINTNEXTLINE
    DepthFunc = depthFunc;
    // NOLINTNEXTLINE
    StencilEnable = stencilEnable;
    // NOLINTNEXTLINE
    StencilReadMask = stencilReadMask;
    // NOLINTNEXTLINE
    StencilWriteMask = stencilWriteMask;
    // NOLINTNEXTLINE
    FrontFace.StencilFailOp = frontStencilFailOp;
    // NOLINTNEXTLINE
    FrontFace.StencilDepthFailOp = frontStencilDepthFailOp;
    // NOLINTNEXTLINE
    FrontFace.StencilPassOp = frontStencilPassOp;
    // NOLINTNEXTLINE
    FrontFace.StencilFunc = frontStencilFunc;
    // NOLINTNEXTLINE
    BackFace.StencilFailOp = backStencilFailOp;
    // NOLINTNEXTLINE
    BackFace.StencilDepthFailOp = backStencilDepthFailOp;
    // NOLINTNEXTLINE
    BackFace.StencilPassOp = backStencilPassOp;
    // NOLINTNEXTLINE
    BackFace.StencilFunc = backStencilFunc;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  ~CD3DX12_DEPTH_STENCIL_DESC() {}
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_DEPTH_STENCIL_DESC1 : public D3D12_DEPTH_STENCIL_DESC1
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_DEPTH_STENCIL_DESC1()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_DEPTH_STENCIL_DESC1( const D3D12_DEPTH_STENCIL_DESC1& o ) :
  // NOLINTNEXTLINE
    D3D12_DEPTH_STENCIL_DESC1( o )
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_DEPTH_STENCIL_DESC1( const D3D12_DEPTH_STENCIL_DESC& o )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    DepthEnable                  = o.DepthEnable;
    // NOLINTNEXTLINE
    DepthWriteMask               = o.DepthWriteMask;
    // NOLINTNEXTLINE
    DepthFunc                    = o.DepthFunc;
    // NOLINTNEXTLINE
    StencilEnable                = o.StencilEnable;
    // NOLINTNEXTLINE
    StencilReadMask              = o.StencilReadMask;
    // NOLINTNEXTLINE
    StencilWriteMask             = o.StencilWriteMask;
    // NOLINTNEXTLINE
    FrontFace.StencilFailOp      = o.FrontFace.StencilFailOp;
    // NOLINTNEXTLINE
    FrontFace.StencilDepthFailOp = o.FrontFace.StencilDepthFailOp;
    // NOLINTNEXTLINE
    FrontFace.StencilPassOp      = o.FrontFace.StencilPassOp;
    // NOLINTNEXTLINE
    FrontFace.StencilFunc        = o.FrontFace.StencilFunc;
    // NOLINTNEXTLINE
    BackFace.StencilFailOp       = o.BackFace.StencilFailOp;
    // NOLINTNEXTLINE
    BackFace.StencilDepthFailOp  = o.BackFace.StencilDepthFailOp;
    // NOLINTNEXTLINE
    BackFace.StencilPassOp       = o.BackFace.StencilPassOp;
    // NOLINTNEXTLINE
    BackFace.StencilFunc         = o.BackFace.StencilFunc;
    // NOLINTNEXTLINE
    DepthBoundsTestEnable        = FALSE;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  explicit CD3DX12_DEPTH_STENCIL_DESC1( CD3DX12_DEFAULT )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    DepthEnable = TRUE;
    // NOLINTNEXTLINE
    DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    // NOLINTNEXTLINE
    DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    // NOLINTNEXTLINE
    StencilEnable = FALSE;
    // NOLINTNEXTLINE
    StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    // NOLINTNEXTLINE
    StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    // NOLINTNEXTLINE
    const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
    // NOLINTNEXTLINE
    { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
    // NOLINTNEXTLINE
    FrontFace = defaultStencilOp;
    // NOLINTNEXTLINE
    BackFace = defaultStencilOp;
    // NOLINTNEXTLINE
    DepthBoundsTestEnable = FALSE;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  explicit CD3DX12_DEPTH_STENCIL_DESC1(
    // NOLINTNEXTLINE
    BOOL depthEnable,
    // NOLINTNEXTLINE
    D3D12_DEPTH_WRITE_MASK depthWriteMask,
    // NOLINTNEXTLINE
    D3D12_COMPARISON_FUNC depthFunc,
    // NOLINTNEXTLINE
    BOOL stencilEnable,
    // NOLINTNEXTLINE
    UINT8 stencilReadMask,
    // NOLINTNEXTLINE
    UINT8 stencilWriteMask,
    // NOLINTNEXTLINE
    D3D12_STENCIL_OP frontStencilFailOp,
    // NOLINTNEXTLINE
    D3D12_STENCIL_OP frontStencilDepthFailOp,
    // NOLINTNEXTLINE
    D3D12_STENCIL_OP frontStencilPassOp,
    // NOLINTNEXTLINE
    D3D12_COMPARISON_FUNC frontStencilFunc,
    // NOLINTNEXTLINE
    D3D12_STENCIL_OP backStencilFailOp,
    // NOLINTNEXTLINE
    D3D12_STENCIL_OP backStencilDepthFailOp,
    // NOLINTNEXTLINE
    D3D12_STENCIL_OP backStencilPassOp,
    // NOLINTNEXTLINE
    D3D12_COMPARISON_FUNC backStencilFunc,
    // NOLINTNEXTLINE
    BOOL depthBoundsTestEnable )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    DepthEnable = depthEnable;
    // NOLINTNEXTLINE
    DepthWriteMask = depthWriteMask;
    // NOLINTNEXTLINE
    DepthFunc = depthFunc;
    // NOLINTNEXTLINE
    StencilEnable = stencilEnable;
    // NOLINTNEXTLINE
    StencilReadMask = stencilReadMask;
    // NOLINTNEXTLINE
    StencilWriteMask = stencilWriteMask;
    // NOLINTNEXTLINE
    FrontFace.StencilFailOp = frontStencilFailOp;
    // NOLINTNEXTLINE
    FrontFace.StencilDepthFailOp = frontStencilDepthFailOp;
    // NOLINTNEXTLINE
    FrontFace.StencilPassOp = frontStencilPassOp;
    // NOLINTNEXTLINE
    FrontFace.StencilFunc = frontStencilFunc;
    // NOLINTNEXTLINE
    BackFace.StencilFailOp = backStencilFailOp;
    // NOLINTNEXTLINE
    BackFace.StencilDepthFailOp = backStencilDepthFailOp;
    // NOLINTNEXTLINE
    BackFace.StencilPassOp = backStencilPassOp;
    // NOLINTNEXTLINE
    BackFace.StencilFunc = backStencilFunc;
    // NOLINTNEXTLINE
    DepthBoundsTestEnable = depthBoundsTestEnable;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  ~CD3DX12_DEPTH_STENCIL_DESC1() {}
  // NOLINTNEXTLINE
  operator D3D12_DEPTH_STENCIL_DESC() const
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    D3D12_DEPTH_STENCIL_DESC D;
    // NOLINTNEXTLINE
    D.DepthEnable                  = DepthEnable;
    // NOLINTNEXTLINE
    D.DepthWriteMask               = DepthWriteMask;
    // NOLINTNEXTLINE
    D.DepthFunc                    = DepthFunc;
    // NOLINTNEXTLINE
    D.StencilEnable                = StencilEnable;
    // NOLINTNEXTLINE
    D.StencilReadMask              = StencilReadMask;
    // NOLINTNEXTLINE
    D.StencilWriteMask             = StencilWriteMask;
    // NOLINTNEXTLINE
    D.FrontFace.StencilFailOp      = FrontFace.StencilFailOp;
    // NOLINTNEXTLINE
    D.FrontFace.StencilDepthFailOp = FrontFace.StencilDepthFailOp;
    // NOLINTNEXTLINE
    D.FrontFace.StencilPassOp      = FrontFace.StencilPassOp;
    // NOLINTNEXTLINE
    D.FrontFace.StencilFunc        = FrontFace.StencilFunc;
    // NOLINTNEXTLINE
    D.BackFace.StencilFailOp       = BackFace.StencilFailOp;
    // NOLINTNEXTLINE
    D.BackFace.StencilDepthFailOp  = BackFace.StencilDepthFailOp;
    // NOLINTNEXTLINE
    D.BackFace.StencilPassOp       = BackFace.StencilPassOp;
    // NOLINTNEXTLINE
    D.BackFace.StencilFunc         = BackFace.StencilFunc;
    // NOLINTNEXTLINE
    return D;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_BLEND_DESC : public D3D12_BLEND_DESC
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_BLEND_DESC()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_BLEND_DESC( const D3D12_BLEND_DESC& o ) :
  // NOLINTNEXTLINE
    D3D12_BLEND_DESC( o )
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_BLEND_DESC( CD3DX12_DEFAULT )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    AlphaToCoverageEnable = FALSE;
    // NOLINTNEXTLINE
    IndependentBlendEnable = FALSE;
    // NOLINTNEXTLINE
    const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
    // NOLINTNEXTLINE
    {
      // NOLINTNEXTLINE
      FALSE,FALSE,
      // NOLINTNEXTLINE
      D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
      // NOLINTNEXTLINE
      D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
      // NOLINTNEXTLINE
      D3D12_LOGIC_OP_NOOP,
      // NOLINTNEXTLINE
      D3D12_COLOR_WRITE_ENABLE_ALL,
      // NOLINTNEXTLINE
    };
    // NOLINTNEXTLINE
    for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
      // NOLINTNEXTLINE
      RenderTarget[ i ] = defaultRenderTargetBlendDesc;
      // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  ~CD3DX12_BLEND_DESC() {}
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_RASTERIZER_DESC : public D3D12_RASTERIZER_DESC
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_RASTERIZER_DESC()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_RASTERIZER_DESC( const D3D12_RASTERIZER_DESC& o ) :
  // NOLINTNEXTLINE
    D3D12_RASTERIZER_DESC( o )
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_RASTERIZER_DESC( CD3DX12_DEFAULT )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    FillMode = D3D12_FILL_MODE_SOLID;
    // NOLINTNEXTLINE
    CullMode = D3D12_CULL_MODE_BACK;
    // NOLINTNEXTLINE
    FrontCounterClockwise = FALSE;
    // NOLINTNEXTLINE
    DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    // NOLINTNEXTLINE
    DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    // NOLINTNEXTLINE
    SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    // NOLINTNEXTLINE
    DepthClipEnable = TRUE;
    // NOLINTNEXTLINE
    MultisampleEnable = FALSE;
    // NOLINTNEXTLINE
    AntialiasedLineEnable = FALSE;
    // NOLINTNEXTLINE
    ForcedSampleCount = 0;
    // NOLINTNEXTLINE
    ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  explicit CD3DX12_RASTERIZER_DESC(
    // NOLINTNEXTLINE
    D3D12_FILL_MODE fillMode,
    // NOLINTNEXTLINE
    D3D12_CULL_MODE cullMode,
    // NOLINTNEXTLINE
    BOOL frontCounterClockwise,
    // NOLINTNEXTLINE
    INT depthBias,
    // NOLINTNEXTLINE
    FLOAT depthBiasClamp,
    // NOLINTNEXTLINE
    FLOAT slopeScaledDepthBias,
    // NOLINTNEXTLINE
    BOOL depthClipEnable,
    // NOLINTNEXTLINE
    BOOL multisampleEnable,
    // NOLINTNEXTLINE
    BOOL antialiasedLineEnable, 
    // NOLINTNEXTLINE
    UINT forcedSampleCount, 
    // NOLINTNEXTLINE
    D3D12_CONSERVATIVE_RASTERIZATION_MODE conservativeRaster)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    FillMode = fillMode;
    // NOLINTNEXTLINE
    CullMode = cullMode;
    // NOLINTNEXTLINE
    FrontCounterClockwise = frontCounterClockwise;
    // NOLINTNEXTLINE
    DepthBias = depthBias;
    // NOLINTNEXTLINE
    DepthBiasClamp = depthBiasClamp;
    // NOLINTNEXTLINE
    SlopeScaledDepthBias = slopeScaledDepthBias;
    // NOLINTNEXTLINE
    DepthClipEnable = depthClipEnable;
    // NOLINTNEXTLINE
    MultisampleEnable = multisampleEnable;
    // NOLINTNEXTLINE
    AntialiasedLineEnable = antialiasedLineEnable;
    // NOLINTNEXTLINE
    ForcedSampleCount = forcedSampleCount;
    // NOLINTNEXTLINE
    ConservativeRaster = conservativeRaster;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  ~CD3DX12_RASTERIZER_DESC() {}
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_RESOURCE_ALLOCATION_INFO : public D3D12_RESOURCE_ALLOCATION_INFO
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_RESOURCE_ALLOCATION_INFO()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_RESOURCE_ALLOCATION_INFO( const D3D12_RESOURCE_ALLOCATION_INFO& o ) :
  // NOLINTNEXTLINE
    D3D12_RESOURCE_ALLOCATION_INFO( o )
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_RESOURCE_ALLOCATION_INFO(
    // NOLINTNEXTLINE
    UINT64 size,
    // NOLINTNEXTLINE
    UINT64 alignment )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    SizeInBytes = size;
    // NOLINTNEXTLINE
    Alignment = alignment;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_HEAP_PROPERTIES : public D3D12_HEAP_PROPERTIES
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_HEAP_PROPERTIES()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_HEAP_PROPERTIES(const D3D12_HEAP_PROPERTIES &o) :
  // NOLINTNEXTLINE
    D3D12_HEAP_PROPERTIES(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_HEAP_PROPERTIES( 
    // NOLINTNEXTLINE
    D3D12_CPU_PAGE_PROPERTY cpuPageProperty, 
    // NOLINTNEXTLINE
    D3D12_MEMORY_POOL memoryPoolPreference,
    // NOLINTNEXTLINE
    UINT creationNodeMask = 1, 
    // NOLINTNEXTLINE
    UINT nodeMask = 1 )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Type = D3D12_HEAP_TYPE_CUSTOM;
    // NOLINTNEXTLINE
    CPUPageProperty = cpuPageProperty;
    // NOLINTNEXTLINE
    MemoryPoolPreference = memoryPoolPreference;
    // NOLINTNEXTLINE
    CreationNodeMask = creationNodeMask;
    // NOLINTNEXTLINE
    VisibleNodeMask = nodeMask;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  explicit CD3DX12_HEAP_PROPERTIES( 
    // NOLINTNEXTLINE
    D3D12_HEAP_TYPE type, 
    // NOLINTNEXTLINE
    UINT creationNodeMask = 1, 
    // NOLINTNEXTLINE
    UINT nodeMask = 1 )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Type = type;
    // NOLINTNEXTLINE
    CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    // NOLINTNEXTLINE
    MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    // NOLINTNEXTLINE
    CreationNodeMask = creationNodeMask;
    // NOLINTNEXTLINE
    VisibleNodeMask = nodeMask;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  bool IsCPUAccessible() const
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    return Type == D3D12_HEAP_TYPE_UPLOAD || Type == D3D12_HEAP_TYPE_READBACK || (Type == D3D12_HEAP_TYPE_CUSTOM &&
      // NOLINTNEXTLINE
      (CPUPageProperty == D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE || CPUPageProperty == D3D12_CPU_PAGE_PROPERTY_WRITE_BACK));
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE
inline bool operator==( const D3D12_HEAP_PROPERTIES& l, const D3D12_HEAP_PROPERTIES& r )
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  return l.Type == r.Type && l.CPUPageProperty == r.CPUPageProperty && 
  // NOLINTNEXTLINE
    l.MemoryPoolPreference == r.MemoryPoolPreference &&
    // NOLINTNEXTLINE
    l.CreationNodeMask == r.CreationNodeMask &&
    // NOLINTNEXTLINE
    l.VisibleNodeMask == r.VisibleNodeMask;
    // NOLINTNEXTLINE
}
// NOLINTNEXTLINE
inline bool operator!=( const D3D12_HEAP_PROPERTIES& l, const D3D12_HEAP_PROPERTIES& r )
// NOLINTNEXTLINE
{ return !( l == r ); }
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_HEAP_DESC : public D3D12_HEAP_DESC
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_HEAP_DESC()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_HEAP_DESC(const D3D12_HEAP_DESC &o) :
  // NOLINTNEXTLINE
    D3D12_HEAP_DESC(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_HEAP_DESC( 
    // NOLINTNEXTLINE
    UINT64 size, 
    // NOLINTNEXTLINE
    D3D12_HEAP_PROPERTIES properties, 
    // NOLINTNEXTLINE
    UINT64 alignment = 0, 
    // NOLINTNEXTLINE
    D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    SizeInBytes = size;
    // NOLINTNEXTLINE
    Properties = properties;
    // NOLINTNEXTLINE
    Alignment = alignment;
    // NOLINTNEXTLINE
    Flags = flags;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  CD3DX12_HEAP_DESC( 
    // NOLINTNEXTLINE
    UINT64 size, 
    // NOLINTNEXTLINE
    D3D12_HEAP_TYPE type, 
    // NOLINTNEXTLINE
    UINT64 alignment = 0, 
    // NOLINTNEXTLINE
    D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    SizeInBytes = size;
    // NOLINTNEXTLINE
    Properties = CD3DX12_HEAP_PROPERTIES( type );
    // NOLINTNEXTLINE
    Alignment = alignment;
    // NOLINTNEXTLINE
    Flags = flags;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  CD3DX12_HEAP_DESC( 
    // NOLINTNEXTLINE
    UINT64 size, 
    // NOLINTNEXTLINE
    D3D12_CPU_PAGE_PROPERTY cpuPageProperty, 
    // NOLINTNEXTLINE
    D3D12_MEMORY_POOL memoryPoolPreference, 
    // NOLINTNEXTLINE
    UINT64 alignment = 0, 
    // NOLINTNEXTLINE
    D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    SizeInBytes = size;
    // NOLINTNEXTLINE
    Properties = CD3DX12_HEAP_PROPERTIES( cpuPageProperty, memoryPoolPreference );
    // NOLINTNEXTLINE
    Alignment = alignment;
    // NOLINTNEXTLINE
    Flags = flags;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  CD3DX12_HEAP_DESC( 
    // NOLINTNEXTLINE
    const D3D12_RESOURCE_ALLOCATION_INFO& resAllocInfo,
    // NOLINTNEXTLINE
    D3D12_HEAP_PROPERTIES properties, 
    // NOLINTNEXTLINE
    D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    SizeInBytes = resAllocInfo.SizeInBytes;
    // NOLINTNEXTLINE
    Properties = properties;
    // NOLINTNEXTLINE
    Alignment = resAllocInfo.Alignment;
    // NOLINTNEXTLINE
    Flags = flags;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  CD3DX12_HEAP_DESC( 
    // NOLINTNEXTLINE
    const D3D12_RESOURCE_ALLOCATION_INFO& resAllocInfo,
    // NOLINTNEXTLINE
    D3D12_HEAP_TYPE type, 
    // NOLINTNEXTLINE
    D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    SizeInBytes = resAllocInfo.SizeInBytes;
    // NOLINTNEXTLINE
    Properties = CD3DX12_HEAP_PROPERTIES( type );
    // NOLINTNEXTLINE
    Alignment = resAllocInfo.Alignment;
    // NOLINTNEXTLINE
    Flags = flags;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  CD3DX12_HEAP_DESC( 
    // NOLINTNEXTLINE
    const D3D12_RESOURCE_ALLOCATION_INFO& resAllocInfo,
    // NOLINTNEXTLINE
    D3D12_CPU_PAGE_PROPERTY cpuPageProperty, 
    // NOLINTNEXTLINE
    D3D12_MEMORY_POOL memoryPoolPreference, 
    // NOLINTNEXTLINE
    D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    SizeInBytes = resAllocInfo.SizeInBytes;
    // NOLINTNEXTLINE
    Properties = CD3DX12_HEAP_PROPERTIES( cpuPageProperty, memoryPoolPreference );
    // NOLINTNEXTLINE
    Alignment = resAllocInfo.Alignment;
    // NOLINTNEXTLINE
    Flags = flags;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  bool IsCPUAccessible() const
  // NOLINTNEXTLINE
  { return static_cast< const CD3DX12_HEAP_PROPERTIES* >( &Properties )->IsCPUAccessible(); }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE
inline bool operator==( const D3D12_HEAP_DESC& l, const D3D12_HEAP_DESC& r )
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  return l.SizeInBytes == r.SizeInBytes &&
  // NOLINTNEXTLINE
    l.Properties == r.Properties && 
    // NOLINTNEXTLINE
    l.Alignment == r.Alignment &&
    // NOLINTNEXTLINE
    l.Flags == r.Flags;
    // NOLINTNEXTLINE
}
// NOLINTNEXTLINE
inline bool operator!=( const D3D12_HEAP_DESC& l, const D3D12_HEAP_DESC& r )
// NOLINTNEXTLINE
{ return !( l == r ); }
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_CLEAR_VALUE : public D3D12_CLEAR_VALUE
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_CLEAR_VALUE()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_CLEAR_VALUE(const D3D12_CLEAR_VALUE &o) :
  // NOLINTNEXTLINE
    D3D12_CLEAR_VALUE(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_CLEAR_VALUE( 
    // NOLINTNEXTLINE
    DXGI_FORMAT format, 
    // NOLINTNEXTLINE
    const FLOAT color[4] )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Format = format;
    // NOLINTNEXTLINE
    memcpy( Color, color, sizeof( Color ) );
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  CD3DX12_CLEAR_VALUE( 
    // NOLINTNEXTLINE
    DXGI_FORMAT format, 
    // NOLINTNEXTLINE
    FLOAT depth,
    // NOLINTNEXTLINE
    UINT8 stencil )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Format = format;
    // NOLINTNEXTLINE
    /* Use memcpy to preserve NAN values */
    // NOLINTNEXTLINE
    memcpy( &DepthStencil.Depth, &depth, sizeof( depth ) );
    // NOLINTNEXTLINE
    DepthStencil.Stencil = stencil;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_RANGE : public D3D12_RANGE
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_RANGE()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_RANGE(const D3D12_RANGE &o) :
  // NOLINTNEXTLINE
    D3D12_RANGE(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_RANGE( 
    // NOLINTNEXTLINE
    SIZE_T begin, 
    // NOLINTNEXTLINE
    SIZE_T end )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Begin = begin;
    // NOLINTNEXTLINE
    End = end;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_RANGE_UINT64 : public D3D12_RANGE_UINT64
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_RANGE_UINT64()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_RANGE_UINT64(const D3D12_RANGE_UINT64 &o) :
  // NOLINTNEXTLINE
    D3D12_RANGE_UINT64(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_RANGE_UINT64( 
    // NOLINTNEXTLINE
    UINT64 begin, 
    // NOLINTNEXTLINE
    UINT64 end )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Begin = begin;
    // NOLINTNEXTLINE
    End = end;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_SUBRESOURCE_RANGE_UINT64 : public D3D12_SUBRESOURCE_RANGE_UINT64
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_SUBRESOURCE_RANGE_UINT64()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_SUBRESOURCE_RANGE_UINT64(const D3D12_SUBRESOURCE_RANGE_UINT64 &o) :
  // NOLINTNEXTLINE
    D3D12_SUBRESOURCE_RANGE_UINT64(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_SUBRESOURCE_RANGE_UINT64( 
    // NOLINTNEXTLINE
    UINT subresource,
    // NOLINTNEXTLINE
    const D3D12_RANGE_UINT64& range )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Subresource = subresource;
    // NOLINTNEXTLINE
    Range = range;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  CD3DX12_SUBRESOURCE_RANGE_UINT64( 
    // NOLINTNEXTLINE
    UINT subresource,
    // NOLINTNEXTLINE
    UINT64 begin, 
    // NOLINTNEXTLINE
    UINT64 end )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Subresource = subresource;
    // NOLINTNEXTLINE
    Range.Begin = begin;
    // NOLINTNEXTLINE
    Range.End = end;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_SHADER_BYTECODE : public D3D12_SHADER_BYTECODE
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_SHADER_BYTECODE()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_SHADER_BYTECODE(const D3D12_SHADER_BYTECODE &o) :
  // NOLINTNEXTLINE
    D3D12_SHADER_BYTECODE(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_SHADER_BYTECODE(
    // NOLINTNEXTLINE
    _In_ ID3DBlob* pShaderBlob )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    pShaderBytecode = pShaderBlob->GetBufferPointer();
    // NOLINTNEXTLINE
    BytecodeLength = pShaderBlob->GetBufferSize();
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  CD3DX12_SHADER_BYTECODE(
    // NOLINTNEXTLINE
    const void* _pShaderBytecode,
    // NOLINTNEXTLINE
    SIZE_T bytecodeLength )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    pShaderBytecode = _pShaderBytecode;
    // NOLINTNEXTLINE
    BytecodeLength = bytecodeLength;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_TILED_RESOURCE_COORDINATE : public D3D12_TILED_RESOURCE_COORDINATE
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_TILED_RESOURCE_COORDINATE()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_TILED_RESOURCE_COORDINATE(const D3D12_TILED_RESOURCE_COORDINATE &o) :
  // NOLINTNEXTLINE
    D3D12_TILED_RESOURCE_COORDINATE(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_TILED_RESOURCE_COORDINATE( 
    // NOLINTNEXTLINE
    UINT x, 
    // NOLINTNEXTLINE
    UINT y, 
    // NOLINTNEXTLINE
    UINT z, 
    // NOLINTNEXTLINE
    UINT subresource ) 
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    X = x;
    // NOLINTNEXTLINE
    Y = y;
    // NOLINTNEXTLINE
    Z = z;
    // NOLINTNEXTLINE
    Subresource = subresource;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_TILE_REGION_SIZE : public D3D12_TILE_REGION_SIZE
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_TILE_REGION_SIZE()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_TILE_REGION_SIZE(const D3D12_TILE_REGION_SIZE &o) :
  // NOLINTNEXTLINE
    D3D12_TILE_REGION_SIZE(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_TILE_REGION_SIZE( 
    // NOLINTNEXTLINE
    UINT numTiles, 
    // NOLINTNEXTLINE
    BOOL useBox, 
    // NOLINTNEXTLINE
    UINT width, 
    // NOLINTNEXTLINE
    UINT16 height, 
    // NOLINTNEXTLINE
    UINT16 depth ) 
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    NumTiles = numTiles;
    // NOLINTNEXTLINE
    UseBox = useBox;
    // NOLINTNEXTLINE
    Width = width;
    // NOLINTNEXTLINE
    Height = height;
    // NOLINTNEXTLINE
    Depth = depth;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_SUBRESOURCE_TILING : public D3D12_SUBRESOURCE_TILING
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_SUBRESOURCE_TILING()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_SUBRESOURCE_TILING(const D3D12_SUBRESOURCE_TILING &o) :
  // NOLINTNEXTLINE
    D3D12_SUBRESOURCE_TILING(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_SUBRESOURCE_TILING( 
    // NOLINTNEXTLINE
    UINT widthInTiles, 
    // NOLINTNEXTLINE
    UINT16 heightInTiles, 
    // NOLINTNEXTLINE
    UINT16 depthInTiles, 
    // NOLINTNEXTLINE
    UINT startTileIndexInOverallResource ) 
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    WidthInTiles = widthInTiles;
    // NOLINTNEXTLINE
    HeightInTiles = heightInTiles;
    // NOLINTNEXTLINE
    DepthInTiles = depthInTiles;
    // NOLINTNEXTLINE
    StartTileIndexInOverallResource = startTileIndexInOverallResource;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_TILE_SHAPE : public D3D12_TILE_SHAPE
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_TILE_SHAPE()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_TILE_SHAPE(const D3D12_TILE_SHAPE &o) :
  // NOLINTNEXTLINE
    D3D12_TILE_SHAPE(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_TILE_SHAPE( 
    // NOLINTNEXTLINE
    UINT widthInTexels, 
    // NOLINTNEXTLINE
    UINT heightInTexels, 
    // NOLINTNEXTLINE
    UINT depthInTexels ) 
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    WidthInTexels = widthInTexels;
    // NOLINTNEXTLINE
    HeightInTexels = heightInTexels;
    // NOLINTNEXTLINE
    DepthInTexels = depthInTexels;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_RESOURCE_BARRIER : public D3D12_RESOURCE_BARRIER
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_RESOURCE_BARRIER()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_RESOURCE_BARRIER(const D3D12_RESOURCE_BARRIER &o) :
  // NOLINTNEXTLINE
    D3D12_RESOURCE_BARRIER(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  static inline CD3DX12_RESOURCE_BARRIER Transition(
    // NOLINTNEXTLINE
    _In_ ID3D12Resource* pResource,
    // NOLINTNEXTLINE
    D3D12_RESOURCE_STATES stateBefore,
    // NOLINTNEXTLINE
    D3D12_RESOURCE_STATES stateAfter,
    // NOLINTNEXTLINE
    UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
    // NOLINTNEXTLINE
    D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    CD3DX12_RESOURCE_BARRIER result;
    // NOLINTNEXTLINE
    ZeroMemory(&result, sizeof(result));
    // NOLINTNEXTLINE
    D3D12_RESOURCE_BARRIER &barrier = result;
    // NOLINTNEXTLINE
    result.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    // NOLINTNEXTLINE
    result.Flags = flags;
    // NOLINTNEXTLINE
    barrier.Transition.pResource = pResource;
    // NOLINTNEXTLINE
    barrier.Transition.StateBefore = stateBefore;
    // NOLINTNEXTLINE
    barrier.Transition.StateAfter = stateAfter;
    // NOLINTNEXTLINE
    barrier.Transition.Subresource = subresource;
    // NOLINTNEXTLINE
    return result;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  static inline CD3DX12_RESOURCE_BARRIER Aliasing(
    // NOLINTNEXTLINE
    _In_ ID3D12Resource* pResourceBefore,
    // NOLINTNEXTLINE
    _In_ ID3D12Resource* pResourceAfter)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    CD3DX12_RESOURCE_BARRIER result;
    // NOLINTNEXTLINE
    ZeroMemory(&result, sizeof(result));
    // NOLINTNEXTLINE
    D3D12_RESOURCE_BARRIER &barrier = result;
    // NOLINTNEXTLINE
    result.Type = D3D12_RESOURCE_BARRIER_TYPE_ALIASING;
    // NOLINTNEXTLINE
    barrier.Aliasing.pResourceBefore = pResourceBefore;
    // NOLINTNEXTLINE
    barrier.Aliasing.pResourceAfter = pResourceAfter;
    // NOLINTNEXTLINE
    return result;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  static inline CD3DX12_RESOURCE_BARRIER UAV(
    // NOLINTNEXTLINE
    _In_ ID3D12Resource* pResource)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    CD3DX12_RESOURCE_BARRIER result;
    // NOLINTNEXTLINE
    ZeroMemory(&result, sizeof(result));
    // NOLINTNEXTLINE
    D3D12_RESOURCE_BARRIER &barrier = result;
    // NOLINTNEXTLINE
    result.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
    // NOLINTNEXTLINE
    barrier.UAV.pResource = pResource;
    // NOLINTNEXTLINE
    return result;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_PACKED_MIP_INFO : public D3D12_PACKED_MIP_INFO
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_PACKED_MIP_INFO()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_PACKED_MIP_INFO(const D3D12_PACKED_MIP_INFO &o) :
  // NOLINTNEXTLINE
    D3D12_PACKED_MIP_INFO(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_PACKED_MIP_INFO( 
    // NOLINTNEXTLINE
    UINT8 numStandardMips, 
    // NOLINTNEXTLINE
    UINT8 numPackedMips, 
    // NOLINTNEXTLINE
    UINT numTilesForPackedMips, 
    // NOLINTNEXTLINE
    UINT startTileIndexInOverallResource ) 
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    NumStandardMips = numStandardMips;
    // NOLINTNEXTLINE
    NumPackedMips = numPackedMips;
    // NOLINTNEXTLINE
    NumTilesForPackedMips = numTilesForPackedMips;
    // NOLINTNEXTLINE
    StartTileIndexInOverallResource = startTileIndexInOverallResource;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_SUBRESOURCE_FOOTPRINT : public D3D12_SUBRESOURCE_FOOTPRINT
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_SUBRESOURCE_FOOTPRINT()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_SUBRESOURCE_FOOTPRINT(const D3D12_SUBRESOURCE_FOOTPRINT &o) :
  // NOLINTNEXTLINE
    D3D12_SUBRESOURCE_FOOTPRINT(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_SUBRESOURCE_FOOTPRINT( 
    // NOLINTNEXTLINE
    DXGI_FORMAT format, 
    // NOLINTNEXTLINE
    UINT width, 
    // NOLINTNEXTLINE
    UINT height, 
    // NOLINTNEXTLINE
    UINT depth, 
    // NOLINTNEXTLINE
    UINT rowPitch ) 
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Format = format;
    // NOLINTNEXTLINE
    Width = width;
    // NOLINTNEXTLINE
    Height = height;
    // NOLINTNEXTLINE
    Depth = depth;
    // NOLINTNEXTLINE
    RowPitch = rowPitch;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  explicit CD3DX12_SUBRESOURCE_FOOTPRINT( 
    // NOLINTNEXTLINE
    const D3D12_RESOURCE_DESC& resDesc, 
    // NOLINTNEXTLINE
    UINT rowPitch ) 
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Format = resDesc.Format;
    // NOLINTNEXTLINE
    Width = UINT( resDesc.Width );
    // NOLINTNEXTLINE
    Height = resDesc.Height;
    // NOLINTNEXTLINE
    Depth = (resDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D ? resDesc.DepthOrArraySize : 1);
    // NOLINTNEXTLINE
    RowPitch = rowPitch;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_TEXTURE_COPY_LOCATION : public D3D12_TEXTURE_COPY_LOCATION
// NOLINTNEXTLINE
{ 
  // NOLINTNEXTLINE
  CD3DX12_TEXTURE_COPY_LOCATION()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_TEXTURE_COPY_LOCATION(const D3D12_TEXTURE_COPY_LOCATION &o) :
  // NOLINTNEXTLINE
    D3D12_TEXTURE_COPY_LOCATION(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_TEXTURE_COPY_LOCATION(ID3D12Resource* pRes) { pResource = pRes; }
  // NOLINTNEXTLINE
  CD3DX12_TEXTURE_COPY_LOCATION(ID3D12Resource* pRes, D3D12_PLACED_SUBRESOURCE_FOOTPRINT const& Footprint)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    pResource = pRes;
    // NOLINTNEXTLINE
    Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    // NOLINTNEXTLINE
    PlacedFootprint = Footprint;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  CD3DX12_TEXTURE_COPY_LOCATION(ID3D12Resource* pRes, UINT Sub)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    pResource = pRes;
    // NOLINTNEXTLINE
    Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    // NOLINTNEXTLINE
    SubresourceIndex = Sub;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
}; 
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_DESCRIPTOR_RANGE : public D3D12_DESCRIPTOR_RANGE
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_DESCRIPTOR_RANGE() { }
  // NOLINTNEXTLINE
  explicit CD3DX12_DESCRIPTOR_RANGE(const D3D12_DESCRIPTOR_RANGE &o) :
  // NOLINTNEXTLINE
    D3D12_DESCRIPTOR_RANGE(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_DESCRIPTOR_RANGE(
    // NOLINTNEXTLINE
    D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
    // NOLINTNEXTLINE
    UINT numDescriptors,
    // NOLINTNEXTLINE
    UINT baseShaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0,
    // NOLINTNEXTLINE
    UINT offsetInDescriptorsFromTableStart =
    // NOLINTNEXTLINE
    D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Init(rangeType, numDescriptors, baseShaderRegister, registerSpace, offsetInDescriptorsFromTableStart);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  inline void Init(
    // NOLINTNEXTLINE
    D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
    // NOLINTNEXTLINE
    UINT numDescriptors,
    // NOLINTNEXTLINE
    UINT baseShaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0,
    // NOLINTNEXTLINE
    UINT offsetInDescriptorsFromTableStart =
    // NOLINTNEXTLINE
    D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Init(*this, rangeType, numDescriptors, baseShaderRegister, registerSpace, offsetInDescriptorsFromTableStart);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void Init(
    // NOLINTNEXTLINE
    _Out_ D3D12_DESCRIPTOR_RANGE &range,
    // NOLINTNEXTLINE
    D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
    // NOLINTNEXTLINE
    UINT numDescriptors,
    // NOLINTNEXTLINE
    UINT baseShaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0,
    // NOLINTNEXTLINE
    UINT offsetInDescriptorsFromTableStart =
    // NOLINTNEXTLINE
    D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    range.RangeType = rangeType;
    // NOLINTNEXTLINE
    range.NumDescriptors = numDescriptors;
    // NOLINTNEXTLINE
    range.BaseShaderRegister = baseShaderRegister;
    // NOLINTNEXTLINE
    range.RegisterSpace = registerSpace;
    // NOLINTNEXTLINE
    range.OffsetInDescriptorsFromTableStart = offsetInDescriptorsFromTableStart;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_ROOT_DESCRIPTOR_TABLE : public D3D12_ROOT_DESCRIPTOR_TABLE
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_ROOT_DESCRIPTOR_TABLE() {}
  // NOLINTNEXTLINE
  explicit CD3DX12_ROOT_DESCRIPTOR_TABLE(const D3D12_ROOT_DESCRIPTOR_TABLE &o) :
  // NOLINTNEXTLINE
    D3D12_ROOT_DESCRIPTOR_TABLE(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_ROOT_DESCRIPTOR_TABLE(
    // NOLINTNEXTLINE
    UINT numDescriptorRanges,
    // NOLINTNEXTLINE
    _In_reads_opt_(numDescriptorRanges) const D3D12_DESCRIPTOR_RANGE* _pDescriptorRanges)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Init(numDescriptorRanges, _pDescriptorRanges);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  inline void Init(
    // NOLINTNEXTLINE
    UINT numDescriptorRanges,
    // NOLINTNEXTLINE
    _In_reads_opt_(numDescriptorRanges) const D3D12_DESCRIPTOR_RANGE* _pDescriptorRanges)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Init(*this, numDescriptorRanges, _pDescriptorRanges);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void Init(
    // NOLINTNEXTLINE
    _Out_ D3D12_ROOT_DESCRIPTOR_TABLE &rootDescriptorTable,
    // NOLINTNEXTLINE
    UINT numDescriptorRanges,
    // NOLINTNEXTLINE
    _In_reads_opt_(numDescriptorRanges) const D3D12_DESCRIPTOR_RANGE* _pDescriptorRanges)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    rootDescriptorTable.NumDescriptorRanges = numDescriptorRanges;
    // NOLINTNEXTLINE
    rootDescriptorTable.pDescriptorRanges = _pDescriptorRanges;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_ROOT_CONSTANTS : public D3D12_ROOT_CONSTANTS
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_ROOT_CONSTANTS() {}
  // NOLINTNEXTLINE
  explicit CD3DX12_ROOT_CONSTANTS(const D3D12_ROOT_CONSTANTS &o) :
  // NOLINTNEXTLINE
    D3D12_ROOT_CONSTANTS(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_ROOT_CONSTANTS(
    // NOLINTNEXTLINE
    UINT num32BitValues,
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Init(num32BitValues, shaderRegister, registerSpace);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  inline void Init(
    // NOLINTNEXTLINE
    UINT num32BitValues,
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Init(*this, num32BitValues, shaderRegister, registerSpace);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void Init(
    // NOLINTNEXTLINE
    _Out_ D3D12_ROOT_CONSTANTS &rootConstants,
    // NOLINTNEXTLINE
    UINT num32BitValues,
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    rootConstants.Num32BitValues = num32BitValues;
    // NOLINTNEXTLINE
    rootConstants.ShaderRegister = shaderRegister;
    // NOLINTNEXTLINE
    rootConstants.RegisterSpace = registerSpace;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_ROOT_DESCRIPTOR : public D3D12_ROOT_DESCRIPTOR
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_ROOT_DESCRIPTOR() {}
  // NOLINTNEXTLINE
  explicit CD3DX12_ROOT_DESCRIPTOR(const D3D12_ROOT_DESCRIPTOR &o) :
  // NOLINTNEXTLINE
    D3D12_ROOT_DESCRIPTOR(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_ROOT_DESCRIPTOR(
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Init(shaderRegister, registerSpace);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  inline void Init(
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Init(*this, shaderRegister, registerSpace);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void Init(_Out_ D3D12_ROOT_DESCRIPTOR &table, UINT shaderRegister, UINT registerSpace = 0)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    table.ShaderRegister = shaderRegister;
    // NOLINTNEXTLINE
    table.RegisterSpace = registerSpace;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_ROOT_PARAMETER : public D3D12_ROOT_PARAMETER
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_ROOT_PARAMETER() {}
  // NOLINTNEXTLINE
  explicit CD3DX12_ROOT_PARAMETER(const D3D12_ROOT_PARAMETER &o) :
  // NOLINTNEXTLINE
    D3D12_ROOT_PARAMETER(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void InitAsDescriptorTable(
    // NOLINTNEXTLINE
    _Out_ D3D12_ROOT_PARAMETER &rootParam,
    // NOLINTNEXTLINE
    UINT numDescriptorRanges,
    // NOLINTNEXTLINE
    _In_reads_(numDescriptorRanges) const D3D12_DESCRIPTOR_RANGE* pDescriptorRanges,
    // NOLINTNEXTLINE
    D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    // NOLINTNEXTLINE
    rootParam.ShaderVisibility = visibility;
    // NOLINTNEXTLINE
    CD3DX12_ROOT_DESCRIPTOR_TABLE::Init(rootParam.DescriptorTable, numDescriptorRanges, pDescriptorRanges);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void InitAsConstants(
    // NOLINTNEXTLINE
    _Out_ D3D12_ROOT_PARAMETER &rootParam,
    // NOLINTNEXTLINE
    UINT num32BitValues,
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0,
    // NOLINTNEXTLINE
    D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    // NOLINTNEXTLINE
    rootParam.ShaderVisibility = visibility;
    // NOLINTNEXTLINE
    CD3DX12_ROOT_CONSTANTS::Init(rootParam.Constants, num32BitValues, shaderRegister, registerSpace);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void InitAsConstantBufferView(
    // NOLINTNEXTLINE
    _Out_ D3D12_ROOT_PARAMETER &rootParam,
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0,
    // NOLINTNEXTLINE
    D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    // NOLINTNEXTLINE
    rootParam.ShaderVisibility = visibility;
    // NOLINTNEXTLINE
    CD3DX12_ROOT_DESCRIPTOR::Init(rootParam.Descriptor, shaderRegister, registerSpace);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void InitAsShaderResourceView(
    // NOLINTNEXTLINE
    _Out_ D3D12_ROOT_PARAMETER &rootParam,
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0,
    // NOLINTNEXTLINE
    D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    // NOLINTNEXTLINE
    rootParam.ShaderVisibility = visibility;
    // NOLINTNEXTLINE
    CD3DX12_ROOT_DESCRIPTOR::Init(rootParam.Descriptor, shaderRegister, registerSpace);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void InitAsUnorderedAccessView(
    // NOLINTNEXTLINE
    _Out_ D3D12_ROOT_PARAMETER &rootParam,
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0,
    // NOLINTNEXTLINE
    D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
    // NOLINTNEXTLINE
    rootParam.ShaderVisibility = visibility;
    // NOLINTNEXTLINE
    CD3DX12_ROOT_DESCRIPTOR::Init(rootParam.Descriptor, shaderRegister, registerSpace);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  inline void InitAsDescriptorTable(
    // NOLINTNEXTLINE
    UINT numDescriptorRanges,
    // NOLINTNEXTLINE
    _In_reads_(numDescriptorRanges) const D3D12_DESCRIPTOR_RANGE* pDescriptorRanges,
    // NOLINTNEXTLINE
    D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    InitAsDescriptorTable(*this, numDescriptorRanges, pDescriptorRanges, visibility);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  inline void InitAsConstants(
    // NOLINTNEXTLINE
    UINT num32BitValues,
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0,
    // NOLINTNEXTLINE
    D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    InitAsConstants(*this, num32BitValues, shaderRegister, registerSpace, visibility);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  inline void InitAsConstantBufferView(
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0,
    // NOLINTNEXTLINE
    D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    InitAsConstantBufferView(*this, shaderRegister, registerSpace, visibility);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  inline void InitAsShaderResourceView(
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0,
    // NOLINTNEXTLINE
    D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    InitAsShaderResourceView(*this, shaderRegister, registerSpace, visibility);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  inline void InitAsUnorderedAccessView(
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0,
    // NOLINTNEXTLINE
    D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    InitAsUnorderedAccessView(*this, shaderRegister, registerSpace, visibility);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_STATIC_SAMPLER_DESC : public D3D12_STATIC_SAMPLER_DESC
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_STATIC_SAMPLER_DESC() {}
  // NOLINTNEXTLINE
  explicit CD3DX12_STATIC_SAMPLER_DESC(const D3D12_STATIC_SAMPLER_DESC &o) :
  // NOLINTNEXTLINE
    D3D12_STATIC_SAMPLER_DESC(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_STATIC_SAMPLER_DESC(
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    D3D12_FILTER filter = D3D12_FILTER_ANISOTROPIC,
    // NOLINTNEXTLINE
    D3D12_TEXTURE_ADDRESS_MODE addressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
    // NOLINTNEXTLINE
    D3D12_TEXTURE_ADDRESS_MODE addressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
    // NOLINTNEXTLINE
    D3D12_TEXTURE_ADDRESS_MODE addressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
    // NOLINTNEXTLINE
    FLOAT mipLODBias = 0,
    // NOLINTNEXTLINE
    UINT maxAnisotropy = 16,
    // NOLINTNEXTLINE
    D3D12_COMPARISON_FUNC comparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL,
    // NOLINTNEXTLINE
    D3D12_STATIC_BORDER_COLOR borderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
    // NOLINTNEXTLINE
    FLOAT minLOD = 0.f,
    // NOLINTNEXTLINE
    FLOAT maxLOD = D3D12_FLOAT32_MAX,
    // NOLINTNEXTLINE
    D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL, 
    // NOLINTNEXTLINE
    UINT registerSpace = 0)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Init(
      // NOLINTNEXTLINE
      shaderRegister,
      // NOLINTNEXTLINE
      filter,
      // NOLINTNEXTLINE
      addressU,
      // NOLINTNEXTLINE
      addressV,
      // NOLINTNEXTLINE
      addressW,
      // NOLINTNEXTLINE
      mipLODBias,
      // NOLINTNEXTLINE
      maxAnisotropy,
      // NOLINTNEXTLINE
      comparisonFunc,
      // NOLINTNEXTLINE
      borderColor,
      // NOLINTNEXTLINE
      minLOD,
      // NOLINTNEXTLINE
      maxLOD,
      // NOLINTNEXTLINE
      shaderVisibility,
      // NOLINTNEXTLINE
      registerSpace);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void Init(
    // NOLINTNEXTLINE
    _Out_ D3D12_STATIC_SAMPLER_DESC &samplerDesc,
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    D3D12_FILTER filter = D3D12_FILTER_ANISOTROPIC,
    // NOLINTNEXTLINE
    D3D12_TEXTURE_ADDRESS_MODE addressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
    // NOLINTNEXTLINE
    D3D12_TEXTURE_ADDRESS_MODE addressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
    // NOLINTNEXTLINE
    D3D12_TEXTURE_ADDRESS_MODE addressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
    // NOLINTNEXTLINE
    FLOAT mipLODBias = 0,
    // NOLINTNEXTLINE
    UINT maxAnisotropy = 16,
    // NOLINTNEXTLINE
    D3D12_COMPARISON_FUNC comparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL,
    // NOLINTNEXTLINE
    D3D12_STATIC_BORDER_COLOR borderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
    // NOLINTNEXTLINE
    FLOAT minLOD = 0.f,
    // NOLINTNEXTLINE
    FLOAT maxLOD = D3D12_FLOAT32_MAX,
    // NOLINTNEXTLINE
    D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL, 
    // NOLINTNEXTLINE
    UINT registerSpace = 0)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    samplerDesc.ShaderRegister = shaderRegister;
    // NOLINTNEXTLINE
    samplerDesc.Filter = filter;
    // NOLINTNEXTLINE
    samplerDesc.AddressU = addressU;
    // NOLINTNEXTLINE
    samplerDesc.AddressV = addressV;
    // NOLINTNEXTLINE
    samplerDesc.AddressW = addressW;
    // NOLINTNEXTLINE
    samplerDesc.MipLODBias = mipLODBias;
    // NOLINTNEXTLINE
    samplerDesc.MaxAnisotropy = maxAnisotropy;
    // NOLINTNEXTLINE
    samplerDesc.ComparisonFunc = comparisonFunc;
    // NOLINTNEXTLINE
    samplerDesc.BorderColor = borderColor;
    // NOLINTNEXTLINE
    samplerDesc.MinLOD = minLOD;
    // NOLINTNEXTLINE
    samplerDesc.MaxLOD = maxLOD;
    // NOLINTNEXTLINE
    samplerDesc.ShaderVisibility = shaderVisibility;
    // NOLINTNEXTLINE
    samplerDesc.RegisterSpace = registerSpace;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  inline void Init(
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    D3D12_FILTER filter = D3D12_FILTER_ANISOTROPIC,
    // NOLINTNEXTLINE
    D3D12_TEXTURE_ADDRESS_MODE addressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
    // NOLINTNEXTLINE
    D3D12_TEXTURE_ADDRESS_MODE addressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
    // NOLINTNEXTLINE
    D3D12_TEXTURE_ADDRESS_MODE addressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
    // NOLINTNEXTLINE
    FLOAT mipLODBias = 0,
    // NOLINTNEXTLINE
    UINT maxAnisotropy = 16,
    // NOLINTNEXTLINE
    D3D12_COMPARISON_FUNC comparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL,
    // NOLINTNEXTLINE
    D3D12_STATIC_BORDER_COLOR borderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
    // NOLINTNEXTLINE
    FLOAT minLOD = 0.f,
    // NOLINTNEXTLINE
    FLOAT maxLOD = D3D12_FLOAT32_MAX,
    // NOLINTNEXTLINE
    D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL, 
    // NOLINTNEXTLINE
    UINT registerSpace = 0)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Init(
      // NOLINTNEXTLINE
      *this,
      // NOLINTNEXTLINE
      shaderRegister,
      // NOLINTNEXTLINE
      filter,
      // NOLINTNEXTLINE
      addressU,
      // NOLINTNEXTLINE
      addressV,
      // NOLINTNEXTLINE
      addressW,
      // NOLINTNEXTLINE
      mipLODBias,
      // NOLINTNEXTLINE
      maxAnisotropy,
      // NOLINTNEXTLINE
      comparisonFunc,
      // NOLINTNEXTLINE
      borderColor,
      // NOLINTNEXTLINE
      minLOD,
      // NOLINTNEXTLINE
      maxLOD,
      // NOLINTNEXTLINE
      shaderVisibility,
      // NOLINTNEXTLINE
      registerSpace);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_ROOT_SIGNATURE_DESC : public D3D12_ROOT_SIGNATURE_DESC
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_ROOT_SIGNATURE_DESC() {}
  // NOLINTNEXTLINE
  explicit CD3DX12_ROOT_SIGNATURE_DESC(const D3D12_ROOT_SIGNATURE_DESC &o) :
  // NOLINTNEXTLINE
    D3D12_ROOT_SIGNATURE_DESC(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_ROOT_SIGNATURE_DESC(
    // NOLINTNEXTLINE
    UINT numParameters,
    // NOLINTNEXTLINE
    _In_reads_opt_(numParameters) const D3D12_ROOT_PARAMETER* _pParameters,
    // NOLINTNEXTLINE
    UINT numStaticSamplers = 0,
    // NOLINTNEXTLINE
    _In_reads_opt_(numStaticSamplers) const D3D12_STATIC_SAMPLER_DESC* _pStaticSamplers = NULL,
    // NOLINTNEXTLINE
    D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Init(numParameters, _pParameters, numStaticSamplers, _pStaticSamplers, flags);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  CD3DX12_ROOT_SIGNATURE_DESC(CD3DX12_DEFAULT)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Init(0, NULL, 0, NULL, D3D12_ROOT_SIGNATURE_FLAG_NONE);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  inline void Init(
    // NOLINTNEXTLINE
    UINT numParameters,
    // NOLINTNEXTLINE
    _In_reads_opt_(numParameters) const D3D12_ROOT_PARAMETER* _pParameters,
    // NOLINTNEXTLINE
    UINT numStaticSamplers = 0,
    // NOLINTNEXTLINE
    _In_reads_opt_(numStaticSamplers) const D3D12_STATIC_SAMPLER_DESC* _pStaticSamplers = NULL,
    // NOLINTNEXTLINE
    D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Init(*this, numParameters, _pParameters, numStaticSamplers, _pStaticSamplers, flags);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void Init(
    // NOLINTNEXTLINE
    _Out_ D3D12_ROOT_SIGNATURE_DESC &desc,
    // NOLINTNEXTLINE
    UINT numParameters,
    // NOLINTNEXTLINE
    _In_reads_opt_(numParameters) const D3D12_ROOT_PARAMETER* _pParameters,
    // NOLINTNEXTLINE
    UINT numStaticSamplers = 0,
    // NOLINTNEXTLINE
    _In_reads_opt_(numStaticSamplers) const D3D12_STATIC_SAMPLER_DESC* _pStaticSamplers = NULL,
    // NOLINTNEXTLINE
    D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    desc.NumParameters = numParameters;
    // NOLINTNEXTLINE
    desc.pParameters = _pParameters;
    // NOLINTNEXTLINE
    desc.NumStaticSamplers = numStaticSamplers;
    // NOLINTNEXTLINE
    desc.pStaticSamplers = _pStaticSamplers;
    // NOLINTNEXTLINE
    desc.Flags = flags;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_DESCRIPTOR_RANGE1 : public D3D12_DESCRIPTOR_RANGE1
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_DESCRIPTOR_RANGE1() { }
  // NOLINTNEXTLINE
  explicit CD3DX12_DESCRIPTOR_RANGE1(const D3D12_DESCRIPTOR_RANGE1 &o) :
  // NOLINTNEXTLINE
    D3D12_DESCRIPTOR_RANGE1(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_DESCRIPTOR_RANGE1(
    // NOLINTNEXTLINE
    D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
    // NOLINTNEXTLINE
    UINT numDescriptors,
    // NOLINTNEXTLINE
    UINT baseShaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0,
    // NOLINTNEXTLINE
    D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
    // NOLINTNEXTLINE
    UINT offsetInDescriptorsFromTableStart =
    // NOLINTNEXTLINE
    D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Init(rangeType, numDescriptors, baseShaderRegister, registerSpace, flags, offsetInDescriptorsFromTableStart);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  inline void Init(
    // NOLINTNEXTLINE
    D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
    // NOLINTNEXTLINE
    UINT numDescriptors,
    // NOLINTNEXTLINE
    UINT baseShaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0,
    // NOLINTNEXTLINE
    D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
    // NOLINTNEXTLINE
    UINT offsetInDescriptorsFromTableStart =
    // NOLINTNEXTLINE
    D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Init(*this, rangeType, numDescriptors, baseShaderRegister, registerSpace, flags, offsetInDescriptorsFromTableStart);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void Init(
    // NOLINTNEXTLINE
    _Out_ D3D12_DESCRIPTOR_RANGE1 &range,
    // NOLINTNEXTLINE
    D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
    // NOLINTNEXTLINE
    UINT numDescriptors,
    // NOLINTNEXTLINE
    UINT baseShaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0,
    // NOLINTNEXTLINE
    D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
    // NOLINTNEXTLINE
    UINT offsetInDescriptorsFromTableStart =
    // NOLINTNEXTLINE
    D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    range.RangeType = rangeType;
    // NOLINTNEXTLINE
    range.NumDescriptors = numDescriptors;
    // NOLINTNEXTLINE
    range.BaseShaderRegister = baseShaderRegister;
    // NOLINTNEXTLINE
    range.RegisterSpace = registerSpace;
    // NOLINTNEXTLINE
    range.Flags = flags;
    // NOLINTNEXTLINE
    range.OffsetInDescriptorsFromTableStart = offsetInDescriptorsFromTableStart;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_ROOT_DESCRIPTOR_TABLE1 : public D3D12_ROOT_DESCRIPTOR_TABLE1
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_ROOT_DESCRIPTOR_TABLE1() {}
  // NOLINTNEXTLINE
  explicit CD3DX12_ROOT_DESCRIPTOR_TABLE1(const D3D12_ROOT_DESCRIPTOR_TABLE1 &o) :
  // NOLINTNEXTLINE
    D3D12_ROOT_DESCRIPTOR_TABLE1(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_ROOT_DESCRIPTOR_TABLE1(
    // NOLINTNEXTLINE
    UINT numDescriptorRanges,
    // NOLINTNEXTLINE
    _In_reads_opt_(numDescriptorRanges) const D3D12_DESCRIPTOR_RANGE1* _pDescriptorRanges)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Init(numDescriptorRanges, _pDescriptorRanges);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  inline void Init(
    // NOLINTNEXTLINE
    UINT numDescriptorRanges,
    // NOLINTNEXTLINE
    _In_reads_opt_(numDescriptorRanges) const D3D12_DESCRIPTOR_RANGE1* _pDescriptorRanges)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Init(*this, numDescriptorRanges, _pDescriptorRanges);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void Init(
    // NOLINTNEXTLINE
    _Out_ D3D12_ROOT_DESCRIPTOR_TABLE1 &rootDescriptorTable,
    // NOLINTNEXTLINE
    UINT numDescriptorRanges,
    // NOLINTNEXTLINE
    _In_reads_opt_(numDescriptorRanges) const D3D12_DESCRIPTOR_RANGE1* _pDescriptorRanges)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    rootDescriptorTable.NumDescriptorRanges = numDescriptorRanges;
    // NOLINTNEXTLINE
    rootDescriptorTable.pDescriptorRanges = _pDescriptorRanges;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_ROOT_DESCRIPTOR1 : public D3D12_ROOT_DESCRIPTOR1
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_ROOT_DESCRIPTOR1() {}
  // NOLINTNEXTLINE
  explicit CD3DX12_ROOT_DESCRIPTOR1(const D3D12_ROOT_DESCRIPTOR1 &o) :
  // NOLINTNEXTLINE
    D3D12_ROOT_DESCRIPTOR1(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_ROOT_DESCRIPTOR1(
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0,
    // NOLINTNEXTLINE
    D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Init(shaderRegister, registerSpace, flags);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  inline void Init(
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0,
    // NOLINTNEXTLINE
    D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Init(*this, shaderRegister, registerSpace, flags);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void Init(
    // NOLINTNEXTLINE
    _Out_ D3D12_ROOT_DESCRIPTOR1 &table, 
    // NOLINTNEXTLINE
    UINT shaderRegister, 
    // NOLINTNEXTLINE
    UINT registerSpace = 0, 
    // NOLINTNEXTLINE
    D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    table.ShaderRegister = shaderRegister;
    // NOLINTNEXTLINE
    table.RegisterSpace = registerSpace;
    // NOLINTNEXTLINE
    table.Flags = flags;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_ROOT_PARAMETER1 : public D3D12_ROOT_PARAMETER1
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_ROOT_PARAMETER1() {}
  // NOLINTNEXTLINE
  explicit CD3DX12_ROOT_PARAMETER1(const D3D12_ROOT_PARAMETER1 &o) :
  // NOLINTNEXTLINE
    D3D12_ROOT_PARAMETER1(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void InitAsDescriptorTable(
    // NOLINTNEXTLINE
    _Out_ D3D12_ROOT_PARAMETER1 &rootParam,
    // NOLINTNEXTLINE
    UINT numDescriptorRanges,
    // NOLINTNEXTLINE
    _In_reads_(numDescriptorRanges) const D3D12_DESCRIPTOR_RANGE1* pDescriptorRanges,
    // NOLINTNEXTLINE
    D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    // NOLINTNEXTLINE
    rootParam.ShaderVisibility = visibility;
    // NOLINTNEXTLINE
    CD3DX12_ROOT_DESCRIPTOR_TABLE1::Init(rootParam.DescriptorTable, numDescriptorRanges, pDescriptorRanges);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void InitAsConstants(
    // NOLINTNEXTLINE
    _Out_ D3D12_ROOT_PARAMETER1 &rootParam,
    // NOLINTNEXTLINE
    UINT num32BitValues,
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0,
    // NOLINTNEXTLINE
    D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    // NOLINTNEXTLINE
    rootParam.ShaderVisibility = visibility;
    // NOLINTNEXTLINE
    CD3DX12_ROOT_CONSTANTS::Init(rootParam.Constants, num32BitValues, shaderRegister, registerSpace);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void InitAsConstantBufferView(
    // NOLINTNEXTLINE
    _Out_ D3D12_ROOT_PARAMETER1 &rootParam,
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0,
    // NOLINTNEXTLINE
    D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
    // NOLINTNEXTLINE
    D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    // NOLINTNEXTLINE
    rootParam.ShaderVisibility = visibility;
    // NOLINTNEXTLINE
    CD3DX12_ROOT_DESCRIPTOR1::Init(rootParam.Descriptor, shaderRegister, registerSpace, flags);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void InitAsShaderResourceView(
    // NOLINTNEXTLINE
    _Out_ D3D12_ROOT_PARAMETER1 &rootParam,
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0,
    // NOLINTNEXTLINE
    D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
    // NOLINTNEXTLINE
    D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
    // NOLINTNEXTLINE
    rootParam.ShaderVisibility = visibility;
    // NOLINTNEXTLINE
    CD3DX12_ROOT_DESCRIPTOR1::Init(rootParam.Descriptor, shaderRegister, registerSpace, flags);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void InitAsUnorderedAccessView(
    // NOLINTNEXTLINE
    _Out_ D3D12_ROOT_PARAMETER1 &rootParam,
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0,
    // NOLINTNEXTLINE
    D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
    // NOLINTNEXTLINE
    D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
    // NOLINTNEXTLINE
    rootParam.ShaderVisibility = visibility;
    // NOLINTNEXTLINE
    CD3DX12_ROOT_DESCRIPTOR1::Init(rootParam.Descriptor, shaderRegister, registerSpace, flags);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  inline void InitAsDescriptorTable(
    // NOLINTNEXTLINE
    UINT numDescriptorRanges,
    // NOLINTNEXTLINE
    _In_reads_(numDescriptorRanges) const D3D12_DESCRIPTOR_RANGE1* pDescriptorRanges,
    // NOLINTNEXTLINE
    D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    InitAsDescriptorTable(*this, numDescriptorRanges, pDescriptorRanges, visibility);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  inline void InitAsConstants(
    // NOLINTNEXTLINE
    UINT num32BitValues,
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0,
    // NOLINTNEXTLINE
    D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    InitAsConstants(*this, num32BitValues, shaderRegister, registerSpace, visibility);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  inline void InitAsConstantBufferView(
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0,
    // NOLINTNEXTLINE
    D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
    // NOLINTNEXTLINE
    D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    InitAsConstantBufferView(*this, shaderRegister, registerSpace, flags, visibility);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  inline void InitAsShaderResourceView(
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0,
    // NOLINTNEXTLINE
    D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
    // NOLINTNEXTLINE
    D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    InitAsShaderResourceView(*this, shaderRegister, registerSpace, flags, visibility);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  inline void InitAsUnorderedAccessView(
    // NOLINTNEXTLINE
    UINT shaderRegister,
    // NOLINTNEXTLINE
    UINT registerSpace = 0,
    // NOLINTNEXTLINE
    D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
    // NOLINTNEXTLINE
    D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    InitAsUnorderedAccessView(*this, shaderRegister, registerSpace, flags, visibility);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC : public D3D12_VERSIONED_ROOT_SIGNATURE_DESC
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC() {}
  // NOLINTNEXTLINE
  explicit CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC(const D3D12_VERSIONED_ROOT_SIGNATURE_DESC &o) :
  // NOLINTNEXTLINE
    D3D12_VERSIONED_ROOT_SIGNATURE_DESC(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC(const D3D12_ROOT_SIGNATURE_DESC &o)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Version = D3D_ROOT_SIGNATURE_VERSION_1_0;
    // NOLINTNEXTLINE
    Desc_1_0 = o;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  explicit CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC(const D3D12_ROOT_SIGNATURE_DESC1 &o)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
    // NOLINTNEXTLINE
    Desc_1_1 = o;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC(
    // NOLINTNEXTLINE
    UINT numParameters,
    // NOLINTNEXTLINE
    _In_reads_opt_(numParameters) const D3D12_ROOT_PARAMETER* _pParameters,
    // NOLINTNEXTLINE
    UINT numStaticSamplers = 0,
    // NOLINTNEXTLINE
    _In_reads_opt_(numStaticSamplers) const D3D12_STATIC_SAMPLER_DESC* _pStaticSamplers = NULL,
    // NOLINTNEXTLINE
    D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Init_1_0(numParameters, _pParameters, numStaticSamplers, _pStaticSamplers, flags);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC(
    // NOLINTNEXTLINE
    UINT numParameters,
    // NOLINTNEXTLINE
    _In_reads_opt_(numParameters) const D3D12_ROOT_PARAMETER1* _pParameters,
    // NOLINTNEXTLINE
    UINT numStaticSamplers = 0,
    // NOLINTNEXTLINE
    _In_reads_opt_(numStaticSamplers) const D3D12_STATIC_SAMPLER_DESC* _pStaticSamplers = NULL,
    // NOLINTNEXTLINE
    D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Init_1_1(numParameters, _pParameters, numStaticSamplers, _pStaticSamplers, flags);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC(CD3DX12_DEFAULT)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Init_1_1(0, NULL, 0, NULL, D3D12_ROOT_SIGNATURE_FLAG_NONE);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  inline void Init_1_0(
    // NOLINTNEXTLINE
    UINT numParameters,
    // NOLINTNEXTLINE
    _In_reads_opt_(numParameters) const D3D12_ROOT_PARAMETER* _pParameters,
    // NOLINTNEXTLINE
    UINT numStaticSamplers = 0,
    // NOLINTNEXTLINE
    _In_reads_opt_(numStaticSamplers) const D3D12_STATIC_SAMPLER_DESC* _pStaticSamplers = NULL,
    // NOLINTNEXTLINE
    D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Init_1_0(*this, numParameters, _pParameters, numStaticSamplers, _pStaticSamplers, flags);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void Init_1_0(
    // NOLINTNEXTLINE
    _Out_ D3D12_VERSIONED_ROOT_SIGNATURE_DESC &desc,
    // NOLINTNEXTLINE
    UINT numParameters,
    // NOLINTNEXTLINE
    _In_reads_opt_(numParameters) const D3D12_ROOT_PARAMETER* _pParameters,
    // NOLINTNEXTLINE
    UINT numStaticSamplers = 0,
    // NOLINTNEXTLINE
    _In_reads_opt_(numStaticSamplers) const D3D12_STATIC_SAMPLER_DESC* _pStaticSamplers = NULL,
    // NOLINTNEXTLINE
    D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    desc.Version = D3D_ROOT_SIGNATURE_VERSION_1_0;
    // NOLINTNEXTLINE
    desc.Desc_1_0.NumParameters = numParameters;
    // NOLINTNEXTLINE
    desc.Desc_1_0.pParameters = _pParameters;
    // NOLINTNEXTLINE
    desc.Desc_1_0.NumStaticSamplers = numStaticSamplers;
    // NOLINTNEXTLINE
    desc.Desc_1_0.pStaticSamplers = _pStaticSamplers;
    // NOLINTNEXTLINE
    desc.Desc_1_0.Flags = flags;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  inline void Init_1_1(
    // NOLINTNEXTLINE
    UINT numParameters,
    // NOLINTNEXTLINE
    _In_reads_opt_(numParameters) const D3D12_ROOT_PARAMETER1* _pParameters,
    // NOLINTNEXTLINE
    UINT numStaticSamplers = 0,
    // NOLINTNEXTLINE
    _In_reads_opt_(numStaticSamplers) const D3D12_STATIC_SAMPLER_DESC* _pStaticSamplers = NULL,
    // NOLINTNEXTLINE
    D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Init_1_1(*this, numParameters, _pParameters, numStaticSamplers, _pStaticSamplers, flags);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void Init_1_1(
    // NOLINTNEXTLINE
    _Out_ D3D12_VERSIONED_ROOT_SIGNATURE_DESC &desc,
    // NOLINTNEXTLINE
    UINT numParameters,
    // NOLINTNEXTLINE
    _In_reads_opt_(numParameters) const D3D12_ROOT_PARAMETER1* _pParameters,
    // NOLINTNEXTLINE
    UINT numStaticSamplers = 0,
    // NOLINTNEXTLINE
    _In_reads_opt_(numStaticSamplers) const D3D12_STATIC_SAMPLER_DESC* _pStaticSamplers = NULL,
    // NOLINTNEXTLINE
    D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    desc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
    // NOLINTNEXTLINE
    desc.Desc_1_1.NumParameters = numParameters;
    // NOLINTNEXTLINE
    desc.Desc_1_1.pParameters = _pParameters;
    // NOLINTNEXTLINE
    desc.Desc_1_1.NumStaticSamplers = numStaticSamplers;
    // NOLINTNEXTLINE
    desc.Desc_1_1.pStaticSamplers = _pStaticSamplers;
    // NOLINTNEXTLINE
    desc.Desc_1_1.Flags = flags;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_CPU_DESCRIPTOR_HANDLE : public D3D12_CPU_DESCRIPTOR_HANDLE
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_CPU_DESCRIPTOR_HANDLE() {}
  // NOLINTNEXTLINE
  explicit CD3DX12_CPU_DESCRIPTOR_HANDLE(const D3D12_CPU_DESCRIPTOR_HANDLE &o) :
  // NOLINTNEXTLINE
    D3D12_CPU_DESCRIPTOR_HANDLE(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_CPU_DESCRIPTOR_HANDLE(CD3DX12_DEFAULT) { ptr = 0; }
  // NOLINTNEXTLINE
  CD3DX12_CPU_DESCRIPTOR_HANDLE(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE &other, INT offsetScaledByIncrementSize)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    InitOffsetted(other, offsetScaledByIncrementSize);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  CD3DX12_CPU_DESCRIPTOR_HANDLE(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE &other, INT offsetInDescriptors, UINT descriptorIncrementSize)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    InitOffsetted(other, offsetInDescriptors, descriptorIncrementSize);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  CD3DX12_CPU_DESCRIPTOR_HANDLE& Offset(INT offsetInDescriptors, UINT descriptorIncrementSize)
  // NOLINTNEXTLINE
  { 
    // NOLINTNEXTLINE
    ptr += offsetInDescriptors * descriptorIncrementSize;
    // NOLINTNEXTLINE
    return *this;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  CD3DX12_CPU_DESCRIPTOR_HANDLE& Offset(INT offsetScaledByIncrementSize) 
  // NOLINTNEXTLINE
  { 
    // NOLINTNEXTLINE
    ptr += offsetScaledByIncrementSize;
    // NOLINTNEXTLINE
    return *this;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  bool operator==(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE& other) const
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    return (ptr == other.ptr);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  bool operator!=(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE& other) const
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    return (ptr != other.ptr);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  CD3DX12_CPU_DESCRIPTOR_HANDLE &operator=(const D3D12_CPU_DESCRIPTOR_HANDLE &other)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    ptr = other.ptr;
    // NOLINTNEXTLINE
    return *this;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  inline void InitOffsetted(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE &base, INT offsetScaledByIncrementSize)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    InitOffsetted(*this, base, offsetScaledByIncrementSize);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  inline void InitOffsetted(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE &base, INT offsetInDescriptors, UINT descriptorIncrementSize)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    InitOffsetted(*this, base, offsetInDescriptors, descriptorIncrementSize);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void InitOffsetted(_Out_ D3D12_CPU_DESCRIPTOR_HANDLE &handle, _In_ const D3D12_CPU_DESCRIPTOR_HANDLE &base, INT offsetScaledByIncrementSize)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    handle.ptr = base.ptr + offsetScaledByIncrementSize;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void InitOffsetted(_Out_ D3D12_CPU_DESCRIPTOR_HANDLE &handle, _In_ const D3D12_CPU_DESCRIPTOR_HANDLE &base, INT offsetInDescriptors, UINT descriptorIncrementSize)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    handle.ptr = base.ptr + offsetInDescriptors * descriptorIncrementSize;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_GPU_DESCRIPTOR_HANDLE : public D3D12_GPU_DESCRIPTOR_HANDLE
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_GPU_DESCRIPTOR_HANDLE() {}
  // NOLINTNEXTLINE
  explicit CD3DX12_GPU_DESCRIPTOR_HANDLE(const D3D12_GPU_DESCRIPTOR_HANDLE &o) :
  // NOLINTNEXTLINE
    D3D12_GPU_DESCRIPTOR_HANDLE(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_GPU_DESCRIPTOR_HANDLE(CD3DX12_DEFAULT) { ptr = 0; }
  // NOLINTNEXTLINE
  CD3DX12_GPU_DESCRIPTOR_HANDLE(_In_ const D3D12_GPU_DESCRIPTOR_HANDLE &other, INT offsetScaledByIncrementSize)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    InitOffsetted(other, offsetScaledByIncrementSize);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  CD3DX12_GPU_DESCRIPTOR_HANDLE(_In_ const D3D12_GPU_DESCRIPTOR_HANDLE &other, INT offsetInDescriptors, UINT descriptorIncrementSize)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    InitOffsetted(other, offsetInDescriptors, descriptorIncrementSize);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  CD3DX12_GPU_DESCRIPTOR_HANDLE& Offset(INT offsetInDescriptors, UINT descriptorIncrementSize)
  // NOLINTNEXTLINE
  { 
    // NOLINTNEXTLINE
    ptr += offsetInDescriptors * descriptorIncrementSize;
    // NOLINTNEXTLINE
    return *this;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  CD3DX12_GPU_DESCRIPTOR_HANDLE& Offset(INT offsetScaledByIncrementSize) 
  // NOLINTNEXTLINE
  { 
    // NOLINTNEXTLINE
    ptr += offsetScaledByIncrementSize;
    // NOLINTNEXTLINE
    return *this;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  inline bool operator==(_In_ const D3D12_GPU_DESCRIPTOR_HANDLE& other) const
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    return (ptr == other.ptr);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  inline bool operator!=(_In_ const D3D12_GPU_DESCRIPTOR_HANDLE& other) const
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    return (ptr != other.ptr);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  CD3DX12_GPU_DESCRIPTOR_HANDLE &operator=(const D3D12_GPU_DESCRIPTOR_HANDLE &other)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    ptr = other.ptr;
    // NOLINTNEXTLINE
    return *this;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  inline void InitOffsetted(_In_ const D3D12_GPU_DESCRIPTOR_HANDLE &base, INT offsetScaledByIncrementSize)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    InitOffsetted(*this, base, offsetScaledByIncrementSize);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  inline void InitOffsetted(_In_ const D3D12_GPU_DESCRIPTOR_HANDLE &base, INT offsetInDescriptors, UINT descriptorIncrementSize)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    InitOffsetted(*this, base, offsetInDescriptors, descriptorIncrementSize);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void InitOffsetted(_Out_ D3D12_GPU_DESCRIPTOR_HANDLE &handle, _In_ const D3D12_GPU_DESCRIPTOR_HANDLE &base, INT offsetScaledByIncrementSize)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    handle.ptr = base.ptr + offsetScaledByIncrementSize;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  static inline void InitOffsetted(_Out_ D3D12_GPU_DESCRIPTOR_HANDLE &handle, _In_ const D3D12_GPU_DESCRIPTOR_HANDLE &base, INT offsetInDescriptors, UINT descriptorIncrementSize)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    handle.ptr = base.ptr + offsetInDescriptors * descriptorIncrementSize;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
inline UINT D3D12CalcSubresource( UINT MipSlice, UINT ArraySlice, UINT PlaneSlice, UINT MipLevels, UINT ArraySize )
// NOLINTNEXTLINE
{ 
  // NOLINTNEXTLINE
  return MipSlice + ArraySlice * MipLevels + PlaneSlice * MipLevels * ArraySize; 
  // NOLINTNEXTLINE
}
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
template <typename T, typename U, typename V>
// NOLINTNEXTLINE
inline void D3D12DecomposeSubresource( UINT Subresource, UINT MipLevels, UINT ArraySize, _Out_ T& MipSlice, _Out_ U& ArraySlice, _Out_ V& PlaneSlice )
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  MipSlice = static_cast<T>(Subresource % MipLevels);
  // NOLINTNEXTLINE
  ArraySlice = static_cast<U>((Subresource / MipLevels) % ArraySize);
  // NOLINTNEXTLINE
  PlaneSlice = static_cast<V>(Subresource / (MipLevels * ArraySize));
  // NOLINTNEXTLINE
}
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
inline UINT8 D3D12GetFormatPlaneCount(
  // NOLINTNEXTLINE
  _In_ ID3D12Device* pDevice,
  // NOLINTNEXTLINE
  DXGI_FORMAT Format
  // NOLINTNEXTLINE
)
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  D3D12_FEATURE_DATA_FORMAT_INFO formatInfo = {Format};
  // NOLINTNEXTLINE
  if (FAILED(pDevice->CheckFeatureSupport(D3D12_FEATURE_FORMAT_INFO, &formatInfo, sizeof(formatInfo))))
    // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    return 0;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  return formatInfo.PlaneCount;
  // NOLINTNEXTLINE
}
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_RESOURCE_DESC : public D3D12_RESOURCE_DESC
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_RESOURCE_DESC()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_RESOURCE_DESC( const D3D12_RESOURCE_DESC& o ) :
  // NOLINTNEXTLINE
    D3D12_RESOURCE_DESC( o )
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_RESOURCE_DESC( 
    // NOLINTNEXTLINE
    D3D12_RESOURCE_DIMENSION dimension,
    // NOLINTNEXTLINE
    UINT64 alignment,
    // NOLINTNEXTLINE
    UINT64 width,
    // NOLINTNEXTLINE
    UINT height,
    // NOLINTNEXTLINE
    UINT16 depthOrArraySize,
    // NOLINTNEXTLINE
    UINT16 mipLevels,
    // NOLINTNEXTLINE
    DXGI_FORMAT format,
    // NOLINTNEXTLINE
    UINT sampleCount,
    // NOLINTNEXTLINE
    UINT sampleQuality,
    // NOLINTNEXTLINE
    D3D12_TEXTURE_LAYOUT layout,
    // NOLINTNEXTLINE
    D3D12_RESOURCE_FLAGS flags )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    Dimension = dimension;
    // NOLINTNEXTLINE
    Alignment = alignment;
    // NOLINTNEXTLINE
    Width = width;
    // NOLINTNEXTLINE
    Height = height;
    // NOLINTNEXTLINE
    DepthOrArraySize = depthOrArraySize;
    // NOLINTNEXTLINE
    MipLevels = mipLevels;
    // NOLINTNEXTLINE
    Format = format;
    // NOLINTNEXTLINE
    SampleDesc.Count = sampleCount;
    // NOLINTNEXTLINE
    SampleDesc.Quality = sampleQuality;
    // NOLINTNEXTLINE
    Layout = layout;
    // NOLINTNEXTLINE
    Flags = flags;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  static inline CD3DX12_RESOURCE_DESC Buffer( 
    // NOLINTNEXTLINE
    const D3D12_RESOURCE_ALLOCATION_INFO& resAllocInfo,
    // NOLINTNEXTLINE
    D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    return CD3DX12_RESOURCE_DESC( D3D12_RESOURCE_DIMENSION_BUFFER, resAllocInfo.Alignment, resAllocInfo.SizeInBytes, 
      // NOLINTNEXTLINE
      1, 1, 1, DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, flags );
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  static inline CD3DX12_RESOURCE_DESC Buffer( 
    // NOLINTNEXTLINE
    UINT64 width,
    // NOLINTNEXTLINE
    D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
    // NOLINTNEXTLINE
    UINT64 alignment = 0 )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    return CD3DX12_RESOURCE_DESC( D3D12_RESOURCE_DIMENSION_BUFFER, alignment, width, 1, 1, 1, 
      // NOLINTNEXTLINE
      DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, flags );
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  static inline CD3DX12_RESOURCE_DESC Tex1D( 
    // NOLINTNEXTLINE
    DXGI_FORMAT format,
    // NOLINTNEXTLINE
    UINT64 width,
    // NOLINTNEXTLINE
    UINT16 arraySize = 1,
    // NOLINTNEXTLINE
    UINT16 mipLevels = 0,
    // NOLINTNEXTLINE
    D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
    // NOLINTNEXTLINE
    D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
    // NOLINTNEXTLINE
    UINT64 alignment = 0 )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    return CD3DX12_RESOURCE_DESC( D3D12_RESOURCE_DIMENSION_TEXTURE1D, alignment, width, 1, arraySize, 
      // NOLINTNEXTLINE
      mipLevels, format, 1, 0, layout, flags );
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  static inline CD3DX12_RESOURCE_DESC Tex2D( 
    // NOLINTNEXTLINE
    DXGI_FORMAT format,
    // NOLINTNEXTLINE
    UINT64 width,
    // NOLINTNEXTLINE
    UINT height,
    // NOLINTNEXTLINE
    UINT16 arraySize = 1,
    // NOLINTNEXTLINE
    UINT16 mipLevels = 0,
    // NOLINTNEXTLINE
    UINT sampleCount = 1,
    // NOLINTNEXTLINE
    UINT sampleQuality = 0,
    // NOLINTNEXTLINE
    D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
    // NOLINTNEXTLINE
    D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
    // NOLINTNEXTLINE
    UINT64 alignment = 0 )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    return CD3DX12_RESOURCE_DESC( D3D12_RESOURCE_DIMENSION_TEXTURE2D, alignment, width, height, arraySize, 
      // NOLINTNEXTLINE
      mipLevels, format, sampleCount, sampleQuality, layout, flags );
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  static inline CD3DX12_RESOURCE_DESC Tex3D( 
    // NOLINTNEXTLINE
    DXGI_FORMAT format,
    // NOLINTNEXTLINE
    UINT64 width,
    // NOLINTNEXTLINE
    UINT height,
    // NOLINTNEXTLINE
    UINT16 depth,
    // NOLINTNEXTLINE
    UINT16 mipLevels = 0,
    // NOLINTNEXTLINE
    D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
    // NOLINTNEXTLINE
    D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
    // NOLINTNEXTLINE
    UINT64 alignment = 0 )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    return CD3DX12_RESOURCE_DESC( D3D12_RESOURCE_DIMENSION_TEXTURE3D, alignment, width, height, depth, 
      // NOLINTNEXTLINE
      mipLevels, format, 1, 0, layout, flags );
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  inline UINT16 Depth() const
  // NOLINTNEXTLINE
  { return (Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D ? DepthOrArraySize : 1); }
  // NOLINTNEXTLINE
  inline UINT16 ArraySize() const
  // NOLINTNEXTLINE
  { return (Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE3D ? DepthOrArraySize : 1); }
  // NOLINTNEXTLINE
  inline UINT8 PlaneCount(_In_ ID3D12Device* pDevice) const
  // NOLINTNEXTLINE
  { return D3D12GetFormatPlaneCount(pDevice, Format); }
  // NOLINTNEXTLINE
  inline UINT Subresources(_In_ ID3D12Device* pDevice) const
  // NOLINTNEXTLINE
  { return MipLevels * ArraySize() * PlaneCount(pDevice); }
  // NOLINTNEXTLINE
  inline UINT CalcSubresource(UINT MipSlice, UINT ArraySlice, UINT PlaneSlice)
  // NOLINTNEXTLINE
  { return D3D12CalcSubresource(MipSlice, ArraySlice, PlaneSlice, MipLevels, ArraySize()); }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE
inline bool operator==( const D3D12_RESOURCE_DESC& l, const D3D12_RESOURCE_DESC& r )
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  return l.Dimension == r.Dimension &&
  // NOLINTNEXTLINE
    l.Alignment == r.Alignment &&
    // NOLINTNEXTLINE
    l.Width == r.Width &&
    // NOLINTNEXTLINE
    l.Height == r.Height &&
    // NOLINTNEXTLINE
    l.DepthOrArraySize == r.DepthOrArraySize &&
    // NOLINTNEXTLINE
    l.MipLevels == r.MipLevels &&
    // NOLINTNEXTLINE
    l.Format == r.Format &&
    // NOLINTNEXTLINE
    l.SampleDesc.Count == r.SampleDesc.Count &&
    // NOLINTNEXTLINE
    l.SampleDesc.Quality == r.SampleDesc.Quality &&
    // NOLINTNEXTLINE
    l.Layout == r.Layout &&
    // NOLINTNEXTLINE
    l.Flags == r.Flags;
    // NOLINTNEXTLINE
}
// NOLINTNEXTLINE
inline bool operator!=( const D3D12_RESOURCE_DESC& l, const D3D12_RESOURCE_DESC& r )
// NOLINTNEXTLINE
{ return !( l == r ); }
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_VIEW_INSTANCING_DESC : public D3D12_VIEW_INSTANCING_DESC
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_VIEW_INSTANCING_DESC()
  // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_VIEW_INSTANCING_DESC( const D3D12_VIEW_INSTANCING_DESC& o ) :
  // NOLINTNEXTLINE
    D3D12_VIEW_INSTANCING_DESC( o )
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_VIEW_INSTANCING_DESC( CD3DX12_DEFAULT )
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    ViewInstanceCount = 0;
    // NOLINTNEXTLINE
    pViewInstanceLocations = nullptr;
    // NOLINTNEXTLINE
    Flags = D3D12_VIEW_INSTANCING_FLAG_NONE;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  explicit CD3DX12_VIEW_INSTANCING_DESC( 
    // NOLINTNEXTLINE
    UINT InViewInstanceCount,
    // NOLINTNEXTLINE
    const D3D12_VIEW_INSTANCE_LOCATION* InViewInstanceLocations,
    // NOLINTNEXTLINE
    D3D12_VIEW_INSTANCING_FLAGS InFlags)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    ViewInstanceCount = InViewInstanceCount;
    // NOLINTNEXTLINE
    pViewInstanceLocations = InViewInstanceLocations;
    // NOLINTNEXTLINE
    Flags = InFlags;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  ~CD3DX12_VIEW_INSTANCING_DESC() {}
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
// Row-by-row memcpy
// NOLINTNEXTLINE
inline void MemcpySubresource(
  // NOLINTNEXTLINE
  _In_ const D3D12_MEMCPY_DEST* pDest,
  // NOLINTNEXTLINE
  _In_ const D3D12_SUBRESOURCE_DATA* pSrc,
  // NOLINTNEXTLINE
  SIZE_T RowSizeInBytes,
  // NOLINTNEXTLINE
  UINT NumRows,
  // NOLINTNEXTLINE
  UINT NumSlices)
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  for (UINT z = 0; z < NumSlices; ++z)
    // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    BYTE* pDestSlice = reinterpret_cast<BYTE*>(pDest->pData) + pDest->SlicePitch * z;
    // NOLINTNEXTLINE
    const BYTE* pSrcSlice = reinterpret_cast<const BYTE*>(pSrc->pData) + pSrc->SlicePitch * z;
    // NOLINTNEXTLINE
    for (UINT y = 0; y < NumRows; ++y)
      // NOLINTNEXTLINE
    {
      // NOLINTNEXTLINE
      memcpy(pDestSlice + pDest->RowPitch * y,
        // NOLINTNEXTLINE
        pSrcSlice + pSrc->RowPitch * y,
        // NOLINTNEXTLINE
        RowSizeInBytes);
      // NOLINTNEXTLINE
    }
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
}
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
// Returns required size of a buffer to be used for data upload
// NOLINTNEXTLINE
inline UINT64 GetRequiredIntermediateSize(
  // NOLINTNEXTLINE
  _In_ ID3D12Resource* pDestinationResource,
  // NOLINTNEXTLINE
  _In_range_(0,D3D12_REQ_SUBRESOURCES) UINT FirstSubresource,
  // NOLINTNEXTLINE
  _In_range_(0,D3D12_REQ_SUBRESOURCES-FirstSubresource) UINT NumSubresources)
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  D3D12_RESOURCE_DESC Desc = pDestinationResource->GetDesc();
  // NOLINTNEXTLINE
  UINT64 RequiredSize = 0;
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  ID3D12Device* pDevice;
  // NOLINTNEXTLINE
  pDestinationResource->GetDevice(__uuidof(*pDevice), reinterpret_cast<void**>(&pDevice));
  // NOLINTNEXTLINE
  pDevice->GetCopyableFootprints(&Desc, FirstSubresource, NumSubresources, 0, nullptr, nullptr, nullptr, &RequiredSize);
  // NOLINTNEXTLINE
  pDevice->Release();
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  return RequiredSize;
  // NOLINTNEXTLINE
}
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
// All arrays must be populated (e.g. by calling GetCopyableFootprints)
// NOLINTNEXTLINE
inline UINT64 UpdateSubresources(
  // NOLINTNEXTLINE
  _In_ ID3D12GraphicsCommandList* pCmdList,
  // NOLINTNEXTLINE
  _In_ ID3D12Resource* pDestinationResource,
  // NOLINTNEXTLINE
  _In_ ID3D12Resource* pIntermediate,
  // NOLINTNEXTLINE
  _In_range_(0,D3D12_REQ_SUBRESOURCES) UINT FirstSubresource,
  // NOLINTNEXTLINE
  _In_range_(0,D3D12_REQ_SUBRESOURCES-FirstSubresource) UINT NumSubresources,
  // NOLINTNEXTLINE
  UINT64 RequiredSize,
  // NOLINTNEXTLINE
  _In_reads_(NumSubresources) const D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts,
  // NOLINTNEXTLINE
  _In_reads_(NumSubresources) const UINT* pNumRows,
  // NOLINTNEXTLINE
  _In_reads_(NumSubresources) const UINT64* pRowSizesInBytes,
  // NOLINTNEXTLINE
  _In_reads_(NumSubresources) const D3D12_SUBRESOURCE_DATA* pSrcData)
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  // Minor validation
  // NOLINTNEXTLINE
  D3D12_RESOURCE_DESC IntermediateDesc = pIntermediate->GetDesc();
  // NOLINTNEXTLINE
  D3D12_RESOURCE_DESC DestinationDesc = pDestinationResource->GetDesc();
  // NOLINTNEXTLINE
  if (IntermediateDesc.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER || 
    // NOLINTNEXTLINE
    IntermediateDesc.Width < RequiredSize + pLayouts[0].Offset || 
    // NOLINTNEXTLINE
    RequiredSize > (SIZE_T)-1 || 
    // NOLINTNEXTLINE
    (DestinationDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER && 
      // NOLINTNEXTLINE
    (FirstSubresource != 0 || NumSubresources != 1)))
    // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    return 0;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  BYTE* pData;
  // NOLINTNEXTLINE
  HRESULT hr = pIntermediate->Map(0, NULL, reinterpret_cast<void**>(&pData));
  // NOLINTNEXTLINE
  if (FAILED(hr))
    // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    return 0;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  for (UINT i = 0; i < NumSubresources; ++i)
    // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    if (pRowSizesInBytes[i] > (SIZE_T)-1) return 0;
    // NOLINTNEXTLINE
    D3D12_MEMCPY_DEST DestData = { pData + pLayouts[i].Offset, pLayouts[i].Footprint.RowPitch, pLayouts[i].Footprint.RowPitch * pNumRows[i] };
    // NOLINTNEXTLINE
    MemcpySubresource(&DestData, &pSrcData[i], (SIZE_T)pRowSizesInBytes[i], pNumRows[i], pLayouts[i].Footprint.Depth);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  pIntermediate->Unmap(0, NULL);
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  if (DestinationDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
    // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    pCmdList->CopyBufferRegion(
      // NOLINTNEXTLINE
      pDestinationResource, 0, pIntermediate, pLayouts[0].Offset, pLayouts[0].Footprint.Width);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  else
    // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    for (UINT i = 0; i < NumSubresources; ++i)
      // NOLINTNEXTLINE
    {
      // NOLINTNEXTLINE
      CD3DX12_TEXTURE_COPY_LOCATION Dst(pDestinationResource, i + FirstSubresource);
      // NOLINTNEXTLINE
      CD3DX12_TEXTURE_COPY_LOCATION Src(pIntermediate, pLayouts[i]);
      // NOLINTNEXTLINE
      pCmdList->CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);
      // NOLINTNEXTLINE
    }
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  return RequiredSize;
  // NOLINTNEXTLINE
}
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
// Heap-allocating UpdateSubresources implementation
// NOLINTNEXTLINE
inline UINT64 UpdateSubresources( 
  // NOLINTNEXTLINE
  _In_ ID3D12GraphicsCommandList* pCmdList,
  // NOLINTNEXTLINE
  _In_ ID3D12Resource* pDestinationResource,
  // NOLINTNEXTLINE
  _In_ ID3D12Resource* pIntermediate,
  // NOLINTNEXTLINE
  UINT64 IntermediateOffset,
  // NOLINTNEXTLINE
  _In_range_(0,D3D12_REQ_SUBRESOURCES) UINT FirstSubresource,
  // NOLINTNEXTLINE
  _In_range_(0,D3D12_REQ_SUBRESOURCES-FirstSubresource) UINT NumSubresources,
  // NOLINTNEXTLINE
  _In_reads_(NumSubresources) D3D12_SUBRESOURCE_DATA* pSrcData)
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  UINT64 RequiredSize = 0;
  // NOLINTNEXTLINE
  UINT64 MemToAlloc = static_cast<UINT64>(sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(UINT) + sizeof(UINT64)) * NumSubresources;
  // NOLINTNEXTLINE
  if (MemToAlloc > SIZE_MAX)
    // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    return 0;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  void* pMem = HeapAlloc(GetProcessHeap(), 0, static_cast<SIZE_T>(MemToAlloc));
  // NOLINTNEXTLINE
  if (pMem == NULL)
    // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    return 0;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts = reinterpret_cast<D3D12_PLACED_SUBRESOURCE_FOOTPRINT*>(pMem);
  // NOLINTNEXTLINE
  UINT64* pRowSizesInBytes = reinterpret_cast<UINT64*>(pLayouts + NumSubresources);
  // NOLINTNEXTLINE
  UINT* pNumRows = reinterpret_cast<UINT*>(pRowSizesInBytes + NumSubresources);
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  D3D12_RESOURCE_DESC Desc = pDestinationResource->GetDesc();
  // NOLINTNEXTLINE
  ID3D12Device* pDevice;
  // NOLINTNEXTLINE
  pDestinationResource->GetDevice(__uuidof(*pDevice), reinterpret_cast<void**>(&pDevice));
  // NOLINTNEXTLINE
  pDevice->GetCopyableFootprints(&Desc, FirstSubresource, NumSubresources, IntermediateOffset, pLayouts, pNumRows, pRowSizesInBytes, &RequiredSize);
  // NOLINTNEXTLINE
  pDevice->Release();
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  UINT64 Result = UpdateSubresources(pCmdList, pDestinationResource, pIntermediate, FirstSubresource, NumSubresources, RequiredSize, pLayouts, pNumRows, pRowSizesInBytes, pSrcData);
  // NOLINTNEXTLINE
  HeapFree(GetProcessHeap(), 0, pMem);
  // NOLINTNEXTLINE
  return Result;
  // NOLINTNEXTLINE
}
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
// Stack-allocating UpdateSubresources implementation
// NOLINTNEXTLINE
template <UINT MaxSubresources>
// NOLINTNEXTLINE
inline UINT64 UpdateSubresources( 
  // NOLINTNEXTLINE
  _In_ ID3D12GraphicsCommandList* pCmdList,
  // NOLINTNEXTLINE
  _In_ ID3D12Resource* pDestinationResource,
  // NOLINTNEXTLINE
  _In_ ID3D12Resource* pIntermediate,
  // NOLINTNEXTLINE
  UINT64 IntermediateOffset,
  // NOLINTNEXTLINE
  _In_range_(0, MaxSubresources) UINT FirstSubresource,
  // NOLINTNEXTLINE
  _In_range_(1, MaxSubresources - FirstSubresource) UINT NumSubresources,
  // NOLINTNEXTLINE
  _In_reads_(NumSubresources) D3D12_SUBRESOURCE_DATA* pSrcData)
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  UINT64 RequiredSize = 0;
  // NOLINTNEXTLINE
  D3D12_PLACED_SUBRESOURCE_FOOTPRINT Layouts[MaxSubresources];
  // NOLINTNEXTLINE
  UINT NumRows[MaxSubresources];
  // NOLINTNEXTLINE
  UINT64 RowSizesInBytes[MaxSubresources];
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  D3D12_RESOURCE_DESC Desc = pDestinationResource->GetDesc();
  // NOLINTNEXTLINE
  ID3D12Device* pDevice;
  // NOLINTNEXTLINE
  pDestinationResource->GetDevice(__uuidof(*pDevice), reinterpret_cast<void**>(&pDevice));
  // NOLINTNEXTLINE
  pDevice->GetCopyableFootprints(&Desc, FirstSubresource, NumSubresources, IntermediateOffset, Layouts, NumRows, RowSizesInBytes, &RequiredSize);
  // NOLINTNEXTLINE
  pDevice->Release();
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  return UpdateSubresources(pCmdList, pDestinationResource, pIntermediate, FirstSubresource, NumSubresources, RequiredSize, Layouts, NumRows, RowSizesInBytes, pSrcData);
  // NOLINTNEXTLINE
}
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
inline bool D3D12IsLayoutOpaque( D3D12_TEXTURE_LAYOUT Layout )
// NOLINTNEXTLINE
{ return Layout == D3D12_TEXTURE_LAYOUT_UNKNOWN || Layout == D3D12_TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE; }
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
template <typename t_CommandListType>
// NOLINTNEXTLINE
inline ID3D12CommandList * const * CommandListCast(t_CommandListType * const * pp)
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  // This cast is useful for passing strongly typed command list pointers into
  // NOLINTNEXTLINE
  // ExecuteCommandLists.
  // NOLINTNEXTLINE
  // This cast is valid as long as the const-ness is respected. D3D12 APIs do
  // NOLINTNEXTLINE
  // respect the const-ness of their arguments.
  // NOLINTNEXTLINE
  return reinterpret_cast<ID3D12CommandList * const *>(pp);
  // NOLINTNEXTLINE
}
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
// D3D12 exports a new method for serializing root signatures in the Windows 10 Anniversary Update.
// NOLINTNEXTLINE
// To help enable root signature 1.1 features when they are available and not require maintaining
// NOLINTNEXTLINE
// two code paths for building root signatures, this helper method reconstructs a 1.0 signature when
// NOLINTNEXTLINE
// 1.1 is not supported.
// NOLINTNEXTLINE
inline HRESULT D3DX12SerializeVersionedRootSignature(
  // NOLINTNEXTLINE
  _In_ const D3D12_VERSIONED_ROOT_SIGNATURE_DESC* pRootSignatureDesc,
  // NOLINTNEXTLINE
  D3D_ROOT_SIGNATURE_VERSION MaxVersion,
  // NOLINTNEXTLINE
  _Outptr_ ID3DBlob** ppBlob,
  // NOLINTNEXTLINE
  _Always_(_Outptr_opt_result_maybenull_) ID3DBlob** ppErrorBlob)
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  if (ppErrorBlob != NULL)
    // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    *ppErrorBlob = NULL;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  switch (MaxVersion)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
  case D3D_ROOT_SIGNATURE_VERSION_1_0:
  // NOLINTNEXTLINE
    switch (pRootSignatureDesc->Version)
    // NOLINTNEXTLINE
    {
      // NOLINTNEXTLINE
    case D3D_ROOT_SIGNATURE_VERSION_1_0:
    // NOLINTNEXTLINE
      return D3D12SerializeRootSignature(&pRootSignatureDesc->Desc_1_0, D3D_ROOT_SIGNATURE_VERSION_1, ppBlob, ppErrorBlob);
      // NOLINTNEXTLINE

// NOLINTNEXTLINE
    case D3D_ROOT_SIGNATURE_VERSION_1_1:
    // NOLINTNEXTLINE
    {
      // NOLINTNEXTLINE
      HRESULT hr = S_OK;
      // NOLINTNEXTLINE
      const D3D12_ROOT_SIGNATURE_DESC1& desc_1_1 = pRootSignatureDesc->Desc_1_1;
      // NOLINTNEXTLINE

// NOLINTNEXTLINE
      const SIZE_T ParametersSize = sizeof(D3D12_ROOT_PARAMETER) * desc_1_1.NumParameters;
      // NOLINTNEXTLINE
      void* pParameters = (ParametersSize > 0) ? HeapAlloc(GetProcessHeap(), 0, ParametersSize) : NULL;
      // NOLINTNEXTLINE
      if (ParametersSize > 0 && pParameters == NULL)
        // NOLINTNEXTLINE
      {
        // NOLINTNEXTLINE
        hr = E_OUTOFMEMORY;
        // NOLINTNEXTLINE
      }
      // NOLINTNEXTLINE
      D3D12_ROOT_PARAMETER* pParameters_1_0 = reinterpret_cast<D3D12_ROOT_PARAMETER*>(pParameters);
      // NOLINTNEXTLINE

// NOLINTNEXTLINE
      if (SUCCEEDED(hr))
        // NOLINTNEXTLINE
      {
        // NOLINTNEXTLINE
        for (UINT n = 0; n < desc_1_1.NumParameters; n++)
          // NOLINTNEXTLINE
        {
          // NOLINTNEXTLINE
          __analysis_assume(ParametersSize == sizeof(D3D12_ROOT_PARAMETER) * desc_1_1.NumParameters);
          // NOLINTNEXTLINE
          pParameters_1_0[n].ParameterType = desc_1_1.pParameters[n].ParameterType;
          // NOLINTNEXTLINE
          pParameters_1_0[n].ShaderVisibility = desc_1_1.pParameters[n].ShaderVisibility;
          // NOLINTNEXTLINE

// NOLINTNEXTLINE
          switch (desc_1_1.pParameters[n].ParameterType)
          // NOLINTNEXTLINE
          {
            // NOLINTNEXTLINE
          case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
          // NOLINTNEXTLINE
            pParameters_1_0[n].Constants.Num32BitValues = desc_1_1.pParameters[n].Constants.Num32BitValues;
            // NOLINTNEXTLINE
            pParameters_1_0[n].Constants.RegisterSpace = desc_1_1.pParameters[n].Constants.RegisterSpace;
            // NOLINTNEXTLINE
            pParameters_1_0[n].Constants.ShaderRegister = desc_1_1.pParameters[n].Constants.ShaderRegister;
            // NOLINTNEXTLINE
            break;
            // NOLINTNEXTLINE

// NOLINTNEXTLINE
          case D3D12_ROOT_PARAMETER_TYPE_CBV:
          // NOLINTNEXTLINE
          case D3D12_ROOT_PARAMETER_TYPE_SRV:
          // NOLINTNEXTLINE
          case D3D12_ROOT_PARAMETER_TYPE_UAV:
          // NOLINTNEXTLINE
            pParameters_1_0[n].Descriptor.RegisterSpace = desc_1_1.pParameters[n].Descriptor.RegisterSpace;
            // NOLINTNEXTLINE
            pParameters_1_0[n].Descriptor.ShaderRegister = desc_1_1.pParameters[n].Descriptor.ShaderRegister;
            // NOLINTNEXTLINE
            break;
            // NOLINTNEXTLINE

// NOLINTNEXTLINE
          case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
          // NOLINTNEXTLINE
            const D3D12_ROOT_DESCRIPTOR_TABLE1& table_1_1 = desc_1_1.pParameters[n].DescriptorTable;
            // NOLINTNEXTLINE

// NOLINTNEXTLINE
            const SIZE_T DescriptorRangesSize = sizeof(D3D12_DESCRIPTOR_RANGE) * table_1_1.NumDescriptorRanges;
            // NOLINTNEXTLINE
            void* pDescriptorRanges = (DescriptorRangesSize > 0 && SUCCEEDED(hr)) ? HeapAlloc(GetProcessHeap(), 0, DescriptorRangesSize) : NULL;
            // NOLINTNEXTLINE
            if (DescriptorRangesSize > 0 && pDescriptorRanges == NULL)
              // NOLINTNEXTLINE
            {
              // NOLINTNEXTLINE
              hr = E_OUTOFMEMORY;
              // NOLINTNEXTLINE
            }
            // NOLINTNEXTLINE
            D3D12_DESCRIPTOR_RANGE* pDescriptorRanges_1_0 = reinterpret_cast<D3D12_DESCRIPTOR_RANGE*>(pDescriptorRanges);
            // NOLINTNEXTLINE

// NOLINTNEXTLINE
            if (SUCCEEDED(hr))
              // NOLINTNEXTLINE
            {
              // NOLINTNEXTLINE
              for (UINT x = 0; x < table_1_1.NumDescriptorRanges; x++)
                // NOLINTNEXTLINE
              {
                // NOLINTNEXTLINE
                __analysis_assume(DescriptorRangesSize == sizeof(D3D12_DESCRIPTOR_RANGE) * table_1_1.NumDescriptorRanges);
                // NOLINTNEXTLINE
                pDescriptorRanges_1_0[x].BaseShaderRegister = table_1_1.pDescriptorRanges[x].BaseShaderRegister;
                // NOLINTNEXTLINE
                pDescriptorRanges_1_0[x].NumDescriptors = table_1_1.pDescriptorRanges[x].NumDescriptors;
                // NOLINTNEXTLINE
                pDescriptorRanges_1_0[x].OffsetInDescriptorsFromTableStart = table_1_1.pDescriptorRanges[x].OffsetInDescriptorsFromTableStart;
                // NOLINTNEXTLINE
                pDescriptorRanges_1_0[x].RangeType = table_1_1.pDescriptorRanges[x].RangeType;
                // NOLINTNEXTLINE
                pDescriptorRanges_1_0[x].RegisterSpace = table_1_1.pDescriptorRanges[x].RegisterSpace;
                // NOLINTNEXTLINE
              }
              // NOLINTNEXTLINE
            }
            // NOLINTNEXTLINE

// NOLINTNEXTLINE
            D3D12_ROOT_DESCRIPTOR_TABLE& table_1_0 = pParameters_1_0[n].DescriptorTable;
            // NOLINTNEXTLINE
            table_1_0.NumDescriptorRanges = table_1_1.NumDescriptorRanges;
            // NOLINTNEXTLINE
            table_1_0.pDescriptorRanges = pDescriptorRanges_1_0;
            // NOLINTNEXTLINE
          }
          // NOLINTNEXTLINE
        }
        // NOLINTNEXTLINE
      }
      // NOLINTNEXTLINE

// NOLINTNEXTLINE
      if (SUCCEEDED(hr))
        // NOLINTNEXTLINE
      {
        // NOLINTNEXTLINE
        CD3DX12_ROOT_SIGNATURE_DESC desc_1_0(desc_1_1.NumParameters, pParameters_1_0, desc_1_1.NumStaticSamplers, desc_1_1.pStaticSamplers, desc_1_1.Flags);
        // NOLINTNEXTLINE
        hr = D3D12SerializeRootSignature(&desc_1_0, D3D_ROOT_SIGNATURE_VERSION_1, ppBlob, ppErrorBlob);
        // NOLINTNEXTLINE
      }
      // NOLINTNEXTLINE

// NOLINTNEXTLINE
      if (pParameters)
        // NOLINTNEXTLINE
      {
        // NOLINTNEXTLINE
        for (UINT n = 0; n < desc_1_1.NumParameters; n++)
          // NOLINTNEXTLINE
        {
          // NOLINTNEXTLINE
          if (desc_1_1.pParameters[n].ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
            // NOLINTNEXTLINE
          {
            // NOLINTNEXTLINE
            HeapFree(GetProcessHeap(), 0, reinterpret_cast<void*>(const_cast<D3D12_DESCRIPTOR_RANGE*>(pParameters_1_0[n].DescriptorTable.pDescriptorRanges)));
            // NOLINTNEXTLINE
          }
          // NOLINTNEXTLINE
        }
        // NOLINTNEXTLINE
        HeapFree(GetProcessHeap(), 0, pParameters);
        // NOLINTNEXTLINE
      }
      // NOLINTNEXTLINE
      return hr;
      // NOLINTNEXTLINE
    }
    // NOLINTNEXTLINE
    }
    // NOLINTNEXTLINE
    break;
    // NOLINTNEXTLINE

// NOLINTNEXTLINE
  case D3D_ROOT_SIGNATURE_VERSION_1_1:
  // NOLINTNEXTLINE
    return D3D12SerializeVersionedRootSignature(pRootSignatureDesc, ppBlob, ppErrorBlob);
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  return E_INVALIDARG;
  // NOLINTNEXTLINE
}
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
struct CD3DX12_RT_FORMAT_ARRAY : public D3D12_RT_FORMAT_ARRAY
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_RT_FORMAT_ARRAY() {}
  // NOLINTNEXTLINE
  explicit CD3DX12_RT_FORMAT_ARRAY(const D3D12_RT_FORMAT_ARRAY& o)
  // NOLINTNEXTLINE
    : D3D12_RT_FORMAT_ARRAY(o)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  explicit CD3DX12_RT_FORMAT_ARRAY(const DXGI_FORMAT* pFormats, UINT NumFormats)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    NumRenderTargets = NumFormats;
    // NOLINTNEXTLINE
    memcpy(RTFormats, pFormats, sizeof(RTFormats));
    // NOLINTNEXTLINE
    // assumes ARRAY_SIZE(pFormats) == ARRAY_SIZE(RTFormats)
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
// Pipeline State Stream Helpers
// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
// Stream Subobjects, i.e. elements of a stream
// NOLINTNEXTLINE

// NOLINTNEXTLINE
struct DefaultSampleMask { operator UINT() { return UINT_MAX; } };
// NOLINTNEXTLINE
struct DefaultSampleDesc { operator DXGI_SAMPLE_DESC() { return DXGI_SAMPLE_DESC{1, 0}; } };
// NOLINTNEXTLINE

// NOLINTNEXTLINE
template <typename InnerStructType, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type, typename DefaultArg = InnerStructType>
// NOLINTNEXTLINE
class alignas(void*) CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
private:
  // NOLINTNEXTLINE
  D3D12_PIPELINE_STATE_SUBOBJECT_TYPE _Type;
  // NOLINTNEXTLINE
  InnerStructType _Inner;
  // NOLINTNEXTLINE
public:
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT() : _Type(Type), _Inner(DefaultArg()) {}
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT(InnerStructType const& i) : _Type(Type), _Inner(i) {}
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT& operator=(InnerStructType const& i) { _Inner = i; return *this; }
  // NOLINTNEXTLINE
  operator InnerStructType() const { return _Inner; }
  // NOLINTNEXTLINE
  operator InnerStructType&() { return _Inner; }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE
typedef CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT< D3D12_PIPELINE_STATE_FLAGS,         D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_FLAGS>                             CD3DX12_PIPELINE_STATE_STREAM_FLAGS;
// NOLINTNEXTLINE
typedef CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT< UINT,                               D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_NODE_MASK>                         CD3DX12_PIPELINE_STATE_STREAM_NODE_MASK;
// NOLINTNEXTLINE
typedef CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT< ID3D12RootSignature*,               D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE>                    CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE;
// NOLINTNEXTLINE
typedef CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT< D3D12_INPUT_LAYOUT_DESC,            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT>                      CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT;
// NOLINTNEXTLINE
typedef CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT< D3D12_INDEX_BUFFER_STRIP_CUT_VALUE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_IB_STRIP_CUT_VALUE>                CD3DX12_PIPELINE_STATE_STREAM_IB_STRIP_CUT_VALUE;
// NOLINTNEXTLINE
typedef CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT< D3D12_PRIMITIVE_TOPOLOGY_TYPE,      D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY>                CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY;
// NOLINTNEXTLINE
typedef CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT< D3D12_SHADER_BYTECODE,              D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS>                                CD3DX12_PIPELINE_STATE_STREAM_VS;
// NOLINTNEXTLINE
typedef CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT< D3D12_SHADER_BYTECODE,              D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_GS>                                CD3DX12_PIPELINE_STATE_STREAM_GS;
// NOLINTNEXTLINE
typedef CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT< D3D12_STREAM_OUTPUT_DESC,           D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_STREAM_OUTPUT>                     CD3DX12_PIPELINE_STATE_STREAM_STREAM_OUTPUT;
// NOLINTNEXTLINE
typedef CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT< D3D12_SHADER_BYTECODE,              D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_HS>                                CD3DX12_PIPELINE_STATE_STREAM_HS;
// NOLINTNEXTLINE
typedef CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT< D3D12_SHADER_BYTECODE,              D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DS>                                CD3DX12_PIPELINE_STATE_STREAM_DS;
// NOLINTNEXTLINE
typedef CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT< D3D12_SHADER_BYTECODE,              D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS>                                CD3DX12_PIPELINE_STATE_STREAM_PS;
// NOLINTNEXTLINE
typedef CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT< D3D12_SHADER_BYTECODE,              D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CS>                                CD3DX12_PIPELINE_STATE_STREAM_CS;
// NOLINTNEXTLINE
typedef CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT< CD3DX12_BLEND_DESC,                 D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND,          CD3DX12_DEFAULT>   CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC;
// NOLINTNEXTLINE
typedef CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT< CD3DX12_DEPTH_STENCIL_DESC,         D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL,  CD3DX12_DEFAULT>   CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL;
// NOLINTNEXTLINE
typedef CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT< CD3DX12_DEPTH_STENCIL_DESC1,        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL1, CD3DX12_DEFAULT>   CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL1;
// NOLINTNEXTLINE
typedef CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT< DXGI_FORMAT,                        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT>              CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT;
// NOLINTNEXTLINE
typedef CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT< CD3DX12_RASTERIZER_DESC,            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER,     CD3DX12_DEFAULT>   CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER;
// NOLINTNEXTLINE
typedef CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT< D3D12_RT_FORMAT_ARRAY,              D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS>             CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS;
// NOLINTNEXTLINE
typedef CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT< DXGI_SAMPLE_DESC,                   D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC,    DefaultSampleDesc> CD3DX12_PIPELINE_STATE_STREAM_SAMPLE_DESC;
// NOLINTNEXTLINE
typedef CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT< UINT,                               D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK,    DefaultSampleMask> CD3DX12_PIPELINE_STATE_STREAM_SAMPLE_MASK;
// NOLINTNEXTLINE
typedef CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT< D3D12_CACHED_PIPELINE_STATE,        D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CACHED_PSO>                        CD3DX12_PIPELINE_STATE_STREAM_CACHED_PSO;
// NOLINTNEXTLINE
typedef CD3DX12_PIPELINE_STATE_STREAM_SUBOBJECT< CD3DX12_VIEW_INSTANCING_DESC,       D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VIEW_INSTANCING, CD3DX12_DEFAULT>  CD3DX12_PIPELINE_STATE_STREAM_VIEW_INSTANCING;
// NOLINTNEXTLINE

// NOLINTNEXTLINE
//------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE
// Stream Parser Helpers
// NOLINTNEXTLINE

// NOLINTNEXTLINE
struct ID3DX12PipelineParserCallbacks
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  // Subobject Callbacks
  // NOLINTNEXTLINE
  virtual void FlagsCb(D3D12_PIPELINE_STATE_FLAGS) {}
  // NOLINTNEXTLINE
  virtual void NodeMaskCb(UINT) {}
  // NOLINTNEXTLINE
  virtual void RootSignatureCb(ID3D12RootSignature*) {}
  // NOLINTNEXTLINE
  virtual void InputLayoutCb(const D3D12_INPUT_LAYOUT_DESC&) {}
  // NOLINTNEXTLINE
  virtual void IBStripCutValueCb(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE) {}
  // NOLINTNEXTLINE
  virtual void PrimitiveTopologyTypeCb(D3D12_PRIMITIVE_TOPOLOGY_TYPE) {}
  // NOLINTNEXTLINE
  virtual void VSCb(const D3D12_SHADER_BYTECODE&) {}
  // NOLINTNEXTLINE
  virtual void GSCb(const D3D12_SHADER_BYTECODE&) {}
  // NOLINTNEXTLINE
  virtual void StreamOutputCb(const D3D12_STREAM_OUTPUT_DESC&) {}
  // NOLINTNEXTLINE
  virtual void HSCb(const D3D12_SHADER_BYTECODE&) {}
  // NOLINTNEXTLINE
  virtual void DSCb(const D3D12_SHADER_BYTECODE&) {}
  // NOLINTNEXTLINE
  virtual void PSCb(const D3D12_SHADER_BYTECODE&) {}
  // NOLINTNEXTLINE
  virtual void CSCb(const D3D12_SHADER_BYTECODE&) {}
  // NOLINTNEXTLINE
  virtual void BlendStateCb(const D3D12_BLEND_DESC&) {}
  // NOLINTNEXTLINE
  virtual void DepthStencilStateCb(const D3D12_DEPTH_STENCIL_DESC&) {}
  // NOLINTNEXTLINE
  virtual void DepthStencilState1Cb(const D3D12_DEPTH_STENCIL_DESC1&) {}
  // NOLINTNEXTLINE
  virtual void DSVFormatCb(DXGI_FORMAT) {}
  // NOLINTNEXTLINE
  virtual void RasterizerStateCb(const D3D12_RASTERIZER_DESC&) {}
  // NOLINTNEXTLINE
  virtual void RTVFormatsCb(const D3D12_RT_FORMAT_ARRAY&) {}
  // NOLINTNEXTLINE
  virtual void SampleDescCb(const DXGI_SAMPLE_DESC&) {}
  // NOLINTNEXTLINE
  virtual void SampleMaskCb(UINT) {}
  // NOLINTNEXTLINE
  virtual void ViewInstancingCb(const D3D12_VIEW_INSTANCING_DESC&) {}
  // NOLINTNEXTLINE
  virtual void CachedPSOCb(const D3D12_CACHED_PIPELINE_STATE&) {}
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  // Error Callbacks
// NOLINTNEXTLINE
  virtual void ErrorBadInputParameter(UINT /*ParameterIndex*/) {}
  // NOLINTNEXTLINE
  virtual void ErrorDuplicateSubobject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE /*DuplicateType*/) {}
  // NOLINTNEXTLINE
  virtual void ErrorUnknownSubobject(UINT /*UnknownTypeValue*/) {}
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
// CD3DX12_PIPELINE_STATE_STREAM1 Works on RS3+ (where there is a new view instancing subobject).  
// NOLINTNEXTLINE
// Use CD3DX12_PIPELINE_STATE_STREAM for RS2+ support.
// NOLINTNEXTLINE
struct CD3DX12_PIPELINE_STATE_STREAM1
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM1() {}
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM1(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& Desc)
  // NOLINTNEXTLINE
    : Flags(Desc.Flags)
    // NOLINTNEXTLINE
    , NodeMask(Desc.NodeMask)
    // NOLINTNEXTLINE
    , pRootSignature(Desc.pRootSignature)
    // NOLINTNEXTLINE
    , InputLayout(Desc.InputLayout)
    // NOLINTNEXTLINE
    , IBStripCutValue(Desc.IBStripCutValue)
    // NOLINTNEXTLINE
    , PrimitiveTopologyType(Desc.PrimitiveTopologyType)
    // NOLINTNEXTLINE
    , VS(Desc.VS)
    // NOLINTNEXTLINE
    , GS(Desc.GS)
    // NOLINTNEXTLINE
    , StreamOutput(Desc.StreamOutput)
    // NOLINTNEXTLINE
    , HS(Desc.HS)
    // NOLINTNEXTLINE
    , DS(Desc.DS)
    // NOLINTNEXTLINE
    , PS(Desc.PS)
    // NOLINTNEXTLINE
    , BlendState(CD3DX12_BLEND_DESC(Desc.BlendState))
    // NOLINTNEXTLINE
    , DepthStencilState(CD3DX12_DEPTH_STENCIL_DESC1(Desc.DepthStencilState))
    // NOLINTNEXTLINE
    , DSVFormat(Desc.DSVFormat)
    // NOLINTNEXTLINE
    , RasterizerState(CD3DX12_RASTERIZER_DESC(Desc.RasterizerState))
    // NOLINTNEXTLINE
    , RTVFormats(CD3DX12_RT_FORMAT_ARRAY(Desc.RTVFormats, Desc.NumRenderTargets))
    // NOLINTNEXTLINE
    , SampleDesc(Desc.SampleDesc)
    // NOLINTNEXTLINE
    , SampleMask(Desc.SampleMask)
    // NOLINTNEXTLINE
    , CachedPSO(Desc.CachedPSO)
    // NOLINTNEXTLINE
    , ViewInstancingDesc(CD3DX12_VIEW_INSTANCING_DESC(CD3DX12_DEFAULT()))
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM1(const D3D12_COMPUTE_PIPELINE_STATE_DESC& Desc)
  // NOLINTNEXTLINE
    : Flags(Desc.Flags)
    // NOLINTNEXTLINE
    , NodeMask(Desc.NodeMask)
    // NOLINTNEXTLINE
    , pRootSignature(Desc.pRootSignature)
    // NOLINTNEXTLINE
    , CS(CD3DX12_SHADER_BYTECODE(Desc.CS))
    // NOLINTNEXTLINE
    , CachedPSO(Desc.CachedPSO)
    // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    static_cast<D3D12_DEPTH_STENCIL_DESC1&>(DepthStencilState).DepthEnable = false;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_FLAGS Flags;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_NODE_MASK NodeMask;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_IB_STRIP_CUT_VALUE IBStripCutValue;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_VS VS;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_GS GS;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_STREAM_OUTPUT StreamOutput;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_HS HS;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_DS DS;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_PS PS;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_CS CS;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC BlendState;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL1 DepthStencilState;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER RasterizerState;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_SAMPLE_DESC SampleDesc;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_SAMPLE_MASK SampleMask;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_CACHED_PSO CachedPSO;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_VIEW_INSTANCING ViewInstancingDesc;
  // NOLINTNEXTLINE
  D3D12_GRAPHICS_PIPELINE_STATE_DESC GraphicsDescV0() const
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    D3D12_GRAPHICS_PIPELINE_STATE_DESC D;
    // NOLINTNEXTLINE
    D.Flags                 = this->Flags;
    // NOLINTNEXTLINE
    D.NodeMask              = this->NodeMask;
    // NOLINTNEXTLINE
    D.pRootSignature        = this->pRootSignature;
    // NOLINTNEXTLINE
    D.InputLayout           = this->InputLayout;
    // NOLINTNEXTLINE
    D.IBStripCutValue       = this->IBStripCutValue;
    // NOLINTNEXTLINE
    D.PrimitiveTopologyType = this->PrimitiveTopologyType;
    // NOLINTNEXTLINE
    D.VS                    = this->VS;
    // NOLINTNEXTLINE
    D.GS                    = this->GS;
    // NOLINTNEXTLINE
    D.StreamOutput          = this->StreamOutput;
    // NOLINTNEXTLINE
    D.HS                    = this->HS;
    // NOLINTNEXTLINE
    D.DS                    = this->DS;
    // NOLINTNEXTLINE
    D.PS                    = this->PS;
    // NOLINTNEXTLINE
    D.BlendState            = this->BlendState;
    // NOLINTNEXTLINE
    D.DepthStencilState     = CD3DX12_DEPTH_STENCIL_DESC1(D3D12_DEPTH_STENCIL_DESC1(this->DepthStencilState));
    // NOLINTNEXTLINE
    D.DSVFormat             = this->DSVFormat;
    // NOLINTNEXTLINE
    D.RasterizerState       = this->RasterizerState;
    // NOLINTNEXTLINE
    D.NumRenderTargets      = D3D12_RT_FORMAT_ARRAY(this->RTVFormats).NumRenderTargets;
    // NOLINTNEXTLINE
    memcpy(D.RTVFormats, D3D12_RT_FORMAT_ARRAY(this->RTVFormats).RTFormats, sizeof(D.RTVFormats));
    // NOLINTNEXTLINE
    D.SampleDesc            = this->SampleDesc;
    // NOLINTNEXTLINE
    D.SampleMask            = this->SampleMask;
    // NOLINTNEXTLINE
    D.CachedPSO             = this->CachedPSO;
    // NOLINTNEXTLINE
    return D;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  D3D12_COMPUTE_PIPELINE_STATE_DESC ComputeDescV0() const
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    D3D12_COMPUTE_PIPELINE_STATE_DESC D;
    // NOLINTNEXTLINE
    D.Flags                 = this->Flags;
    // NOLINTNEXTLINE
    D.NodeMask              = this->NodeMask;
    // NOLINTNEXTLINE
    D.pRootSignature        = this->pRootSignature;
    // NOLINTNEXTLINE
    D.CS                    = this->CS;
    // NOLINTNEXTLINE
    D.CachedPSO             = this->CachedPSO;
    // NOLINTNEXTLINE
    return D;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
// CD3DX12_PIPELINE_STATE_STREAM works on RS2+ but does not support new subobject(s) added in RS3+.
// NOLINTNEXTLINE
// See CD3DX12_PIPELINE_STATE_STREAM1 for instance.
// NOLINTNEXTLINE
struct CD3DX12_PIPELINE_STATE_STREAM
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM() {}
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& Desc)
  // NOLINTNEXTLINE
    : Flags(Desc.Flags)
    // NOLINTNEXTLINE
    , NodeMask(Desc.NodeMask)
    // NOLINTNEXTLINE
    , pRootSignature(Desc.pRootSignature)
    // NOLINTNEXTLINE
    , InputLayout(Desc.InputLayout)
    // NOLINTNEXTLINE
    , IBStripCutValue(Desc.IBStripCutValue)
    // NOLINTNEXTLINE
    , PrimitiveTopologyType(Desc.PrimitiveTopologyType)
    // NOLINTNEXTLINE
    , VS(Desc.VS)
    // NOLINTNEXTLINE
    , GS(Desc.GS)
    // NOLINTNEXTLINE
    , StreamOutput(Desc.StreamOutput)
    // NOLINTNEXTLINE
    , HS(Desc.HS)
    // NOLINTNEXTLINE
    , DS(Desc.DS)
    // NOLINTNEXTLINE
    , PS(Desc.PS)
    // NOLINTNEXTLINE
    , BlendState(CD3DX12_BLEND_DESC(Desc.BlendState))
    // NOLINTNEXTLINE
    , DepthStencilState(CD3DX12_DEPTH_STENCIL_DESC1(Desc.DepthStencilState))
    // NOLINTNEXTLINE
    , DSVFormat(Desc.DSVFormat)
    // NOLINTNEXTLINE
    , RasterizerState(CD3DX12_RASTERIZER_DESC(Desc.RasterizerState))
    // NOLINTNEXTLINE
    , RTVFormats(CD3DX12_RT_FORMAT_ARRAY(Desc.RTVFormats, Desc.NumRenderTargets))
    // NOLINTNEXTLINE
    , SampleDesc(Desc.SampleDesc)
    // NOLINTNEXTLINE
    , SampleMask(Desc.SampleMask)
    // NOLINTNEXTLINE
    , CachedPSO(Desc.CachedPSO)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM(const D3D12_COMPUTE_PIPELINE_STATE_DESC& Desc)
  // NOLINTNEXTLINE
    : Flags(Desc.Flags)
    // NOLINTNEXTLINE
    , NodeMask(Desc.NodeMask)
    // NOLINTNEXTLINE
    , pRootSignature(Desc.pRootSignature)
    // NOLINTNEXTLINE
    , CS(CD3DX12_SHADER_BYTECODE(Desc.CS))
    // NOLINTNEXTLINE
    , CachedPSO(Desc.CachedPSO)
    // NOLINTNEXTLINE
  {}
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_FLAGS Flags;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_NODE_MASK NodeMask;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_IB_STRIP_CUT_VALUE IBStripCutValue;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_VS VS;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_GS GS;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_STREAM_OUTPUT StreamOutput;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_HS HS;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_DS DS;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_PS PS;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_CS CS;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC BlendState;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL1 DepthStencilState;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER RasterizerState;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_SAMPLE_DESC SampleDesc;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_SAMPLE_MASK SampleMask;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_CACHED_PSO CachedPSO;
  // NOLINTNEXTLINE
  D3D12_GRAPHICS_PIPELINE_STATE_DESC GraphicsDescV0() const
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    D3D12_GRAPHICS_PIPELINE_STATE_DESC D;
    // NOLINTNEXTLINE
    D.Flags                 = this->Flags;
    // NOLINTNEXTLINE
    D.NodeMask              = this->NodeMask;
    // NOLINTNEXTLINE
    D.pRootSignature        = this->pRootSignature;
    // NOLINTNEXTLINE
    D.InputLayout           = this->InputLayout;
    // NOLINTNEXTLINE
    D.IBStripCutValue       = this->IBStripCutValue;
    // NOLINTNEXTLINE
    D.PrimitiveTopologyType = this->PrimitiveTopologyType;
    // NOLINTNEXTLINE
    D.VS                    = this->VS;
    // NOLINTNEXTLINE
    D.GS                    = this->GS;
    // NOLINTNEXTLINE
    D.StreamOutput          = this->StreamOutput;
    // NOLINTNEXTLINE
    D.HS                    = this->HS;
    // NOLINTNEXTLINE
    D.DS                    = this->DS;
    // NOLINTNEXTLINE
    D.PS                    = this->PS;
    // NOLINTNEXTLINE
    D.BlendState            = this->BlendState;
    // NOLINTNEXTLINE
    D.DepthStencilState     = CD3DX12_DEPTH_STENCIL_DESC1(D3D12_DEPTH_STENCIL_DESC1(this->DepthStencilState));
    // NOLINTNEXTLINE
    D.DSVFormat             = this->DSVFormat;
    // NOLINTNEXTLINE
    D.RasterizerState       = this->RasterizerState;
    // NOLINTNEXTLINE
    D.NumRenderTargets      = D3D12_RT_FORMAT_ARRAY(this->RTVFormats).NumRenderTargets;
    // NOLINTNEXTLINE
    memcpy(D.RTVFormats, D3D12_RT_FORMAT_ARRAY(this->RTVFormats).RTFormats, sizeof(D.RTVFormats));
    // NOLINTNEXTLINE
    D.SampleDesc            = this->SampleDesc;
    // NOLINTNEXTLINE
    D.SampleMask            = this->SampleMask;
    // NOLINTNEXTLINE
    D.CachedPSO             = this->CachedPSO;
    // NOLINTNEXTLINE
    return D;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  D3D12_COMPUTE_PIPELINE_STATE_DESC ComputeDescV0() const
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    D3D12_COMPUTE_PIPELINE_STATE_DESC D;
    // NOLINTNEXTLINE
    D.Flags                 = this->Flags;
    // NOLINTNEXTLINE
    D.NodeMask              = this->NodeMask;
    // NOLINTNEXTLINE
    D.pRootSignature        = this->pRootSignature;
    // NOLINTNEXTLINE
    D.CS                    = this->CS;
    // NOLINTNEXTLINE
    D.CachedPSO             = this->CachedPSO;
    // NOLINTNEXTLINE
    return D;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
struct CD3DX12_PIPELINE_STATE_STREAM_PARSE_HELPER : public ID3DX12PipelineParserCallbacks
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM1 PipelineStream;
  // NOLINTNEXTLINE
  CD3DX12_PIPELINE_STATE_STREAM_PARSE_HELPER()
  // NOLINTNEXTLINE
    : SeenDSS(false)
    // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    // Adjust defaults to account for absent members.
    // NOLINTNEXTLINE
    PipelineStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    // NOLINTNEXTLINE

// NOLINTNEXTLINE
    // Depth disabled if no DSV format specified.
// NOLINTNEXTLINE
    static_cast<D3D12_DEPTH_STENCIL_DESC1&>(PipelineStream.DepthStencilState).DepthEnable = false;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  virtual ~CD3DX12_PIPELINE_STATE_STREAM_PARSE_HELPER() { }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  // ID3DX12PipelineParserCallbacks
// NOLINTNEXTLINE
  void FlagsCb(D3D12_PIPELINE_STATE_FLAGS Flags) {PipelineStream.Flags = Flags;}
  // NOLINTNEXTLINE
  void NodeMaskCb(UINT NodeMask) {PipelineStream.NodeMask = NodeMask;}
  // NOLINTNEXTLINE
  void RootSignatureCb(ID3D12RootSignature* pRootSignature) {PipelineStream.pRootSignature = pRootSignature;}
  // NOLINTNEXTLINE
  void InputLayoutCb(const D3D12_INPUT_LAYOUT_DESC& InputLayout) {PipelineStream.InputLayout = InputLayout;}
  // NOLINTNEXTLINE
  void IBStripCutValueCb(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBStripCutValue) {PipelineStream.IBStripCutValue = IBStripCutValue;}
  // NOLINTNEXTLINE
  void PrimitiveTopologyTypeCb(D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType) {PipelineStream.PrimitiveTopologyType = PrimitiveTopologyType;}
  // NOLINTNEXTLINE
  void VSCb(const D3D12_SHADER_BYTECODE& VS) {PipelineStream.VS = VS;}
  // NOLINTNEXTLINE
  void GSCb(const D3D12_SHADER_BYTECODE& GS) {PipelineStream.GS = GS;}
  // NOLINTNEXTLINE
  void StreamOutputCb(const D3D12_STREAM_OUTPUT_DESC& StreamOutput) {PipelineStream.StreamOutput = StreamOutput;}
  // NOLINTNEXTLINE
  void HSCb(const D3D12_SHADER_BYTECODE& HS) {PipelineStream.HS = HS;}
  // NOLINTNEXTLINE
  void DSCb(const D3D12_SHADER_BYTECODE& DS) {PipelineStream.DS = DS;}
  // NOLINTNEXTLINE
  void PSCb(const D3D12_SHADER_BYTECODE& PS) {PipelineStream.PS = PS;}
  // NOLINTNEXTLINE
  void CSCb(const D3D12_SHADER_BYTECODE& CS) {PipelineStream.CS = CS;}
  // NOLINTNEXTLINE
  void BlendStateCb(const D3D12_BLEND_DESC& BlendState) {PipelineStream.BlendState = CD3DX12_BLEND_DESC(BlendState);}
  // NOLINTNEXTLINE
  void DepthStencilStateCb(const D3D12_DEPTH_STENCIL_DESC& DepthStencilState)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    PipelineStream.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC1(DepthStencilState);
    // NOLINTNEXTLINE
    SeenDSS = true;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  void DepthStencilState1Cb(const D3D12_DEPTH_STENCIL_DESC1& DepthStencilState)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    PipelineStream.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC1(DepthStencilState);
    // NOLINTNEXTLINE
    SeenDSS = true;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  void DSVFormatCb(DXGI_FORMAT DSVFormat)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    PipelineStream.DSVFormat = DSVFormat;
    // NOLINTNEXTLINE
    if (!SeenDSS && DSVFormat != DXGI_FORMAT_UNKNOWN)
      // NOLINTNEXTLINE
    {
      // NOLINTNEXTLINE
      // Re-enable depth for the default state.
      // NOLINTNEXTLINE
      static_cast<D3D12_DEPTH_STENCIL_DESC1&>(PipelineStream.DepthStencilState).DepthEnable = true;
      // NOLINTNEXTLINE
    }
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
  void RasterizerStateCb(const D3D12_RASTERIZER_DESC& RasterizerState) {PipelineStream.RasterizerState = CD3DX12_RASTERIZER_DESC(RasterizerState);}
  // NOLINTNEXTLINE
  void RTVFormatsCb(const D3D12_RT_FORMAT_ARRAY& RTVFormats) {PipelineStream.RTVFormats = RTVFormats;}
  // NOLINTNEXTLINE
  void SampleDescCb(const DXGI_SAMPLE_DESC& SampleDesc) {PipelineStream.SampleDesc = SampleDesc;}
  // NOLINTNEXTLINE
  void SampleMaskCb(UINT SampleMask) {PipelineStream.SampleMask = SampleMask;}
  // NOLINTNEXTLINE
  void ViewInstancingCb(const D3D12_VIEW_INSTANCING_DESC& ViewInstancingDesc) {PipelineStream.ViewInstancingDesc = CD3DX12_VIEW_INSTANCING_DESC(ViewInstancingDesc);}
  // NOLINTNEXTLINE
  void CachedPSOCb(const D3D12_CACHED_PIPELINE_STATE& CachedPSO) {PipelineStream.CachedPSO = CachedPSO;}
  // NOLINTNEXTLINE
  void ErrorBadInputParameter(UINT) {}
  // NOLINTNEXTLINE
  void ErrorDuplicateSubobject(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE) {}
  // NOLINTNEXTLINE
  void ErrorUnknownSubobject(UINT) {}
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
private:
  // NOLINTNEXTLINE
  bool SeenDSS;
  // NOLINTNEXTLINE
};
// NOLINTNEXTLINE

// NOLINTNEXTLINE
inline D3D12_PIPELINE_STATE_SUBOBJECT_TYPE D3DX12GetBaseSubobjectType(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE SubobjectType)
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  switch (SubobjectType)
  // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
  case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL1: 
  // NOLINTNEXTLINE
    return D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL;
    // NOLINTNEXTLINE
  default:
  // NOLINTNEXTLINE
    return SubobjectType;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE
}
// NOLINTNEXTLINE

// NOLINTNEXTLINE
inline HRESULT D3DX12ParsePipelineStream(const D3D12_PIPELINE_STATE_STREAM_DESC& Desc, ID3DX12PipelineParserCallbacks* pCallbacks)
// NOLINTNEXTLINE
{
  // NOLINTNEXTLINE
  if (pCallbacks == nullptr)
    // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    return E_INVALIDARG;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  if (Desc.SizeInBytes == 0 || Desc.pPipelineStateSubobjectStream == nullptr)
    // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    pCallbacks->ErrorBadInputParameter(1); // first parameter issue
    // NOLINTNEXTLINE
    return E_INVALIDARG;
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  bool SubobjectSeen[D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MAX_VALID] = {0};
  // NOLINTNEXTLINE
  for (SIZE_T CurOffset = 0, SizeOfSubobject = 0; CurOffset < Desc.SizeInBytes; CurOffset += SizeOfSubobject)
    // NOLINTNEXTLINE
  {
    // NOLINTNEXTLINE
    BYTE* pStream = static_cast<BYTE*>(Desc.pPipelineStateSubobjectStream)+CurOffset;
    // NOLINTNEXTLINE
    auto SubobjectType = *reinterpret_cast<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE*>(pStream);
    // NOLINTNEXTLINE
    if (SubobjectType >= D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MAX_VALID)
      // NOLINTNEXTLINE
    {
      // NOLINTNEXTLINE
      pCallbacks->ErrorUnknownSubobject(SubobjectType);
      // NOLINTNEXTLINE
      return E_INVALIDARG;
      // NOLINTNEXTLINE
    }
    // NOLINTNEXTLINE
    if (SubobjectSeen[D3DX12GetBaseSubobjectType(SubobjectType)])
      // NOLINTNEXTLINE
    {
      // NOLINTNEXTLINE
      pCallbacks->ErrorDuplicateSubobject(SubobjectType);
      // NOLINTNEXTLINE
      return E_INVALIDARG; // disallow subobject duplicates in a stream
      // NOLINTNEXTLINE
    }
    // NOLINTNEXTLINE
    SubobjectSeen[SubobjectType] = true;
    // NOLINTNEXTLINE
    switch (SubobjectType)
    // NOLINTNEXTLINE
    {
      // NOLINTNEXTLINE
    case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE: 
    // NOLINTNEXTLINE
      pCallbacks->RootSignatureCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::pRootSignature)*>(pStream));
      // NOLINTNEXTLINE
      SizeOfSubobject = sizeof(CD3DX12_PIPELINE_STATE_STREAM::pRootSignature);
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS:
    // NOLINTNEXTLINE
      pCallbacks->VSCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::VS)*>(pStream));
      // NOLINTNEXTLINE
      SizeOfSubobject = sizeof(CD3DX12_PIPELINE_STATE_STREAM::VS);
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS: 
    // NOLINTNEXTLINE
      pCallbacks->PSCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::PS)*>(pStream));
      // NOLINTNEXTLINE
      SizeOfSubobject = sizeof(CD3DX12_PIPELINE_STATE_STREAM::PS);
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DS: 
    // NOLINTNEXTLINE
      pCallbacks->DSCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::DS)*>(pStream));
      // NOLINTNEXTLINE
      SizeOfSubobject = sizeof(CD3DX12_PIPELINE_STATE_STREAM::DS);
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_HS: 
    // NOLINTNEXTLINE
      pCallbacks->HSCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::HS)*>(pStream));
      // NOLINTNEXTLINE
      SizeOfSubobject = sizeof(CD3DX12_PIPELINE_STATE_STREAM::HS);
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_GS: 
    // NOLINTNEXTLINE
      pCallbacks->GSCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::GS)*>(pStream));
      // NOLINTNEXTLINE
      SizeOfSubobject = sizeof(CD3DX12_PIPELINE_STATE_STREAM::GS);
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CS:
    // NOLINTNEXTLINE
      pCallbacks->CSCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::CS)*>(pStream));
      // NOLINTNEXTLINE
      SizeOfSubobject = sizeof(CD3DX12_PIPELINE_STATE_STREAM::CS);
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_STREAM_OUTPUT: 
    // NOLINTNEXTLINE
      pCallbacks->StreamOutputCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::StreamOutput)*>(pStream));
      // NOLINTNEXTLINE
      SizeOfSubobject = sizeof(CD3DX12_PIPELINE_STATE_STREAM::StreamOutput);
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND: 
    // NOLINTNEXTLINE
      pCallbacks->BlendStateCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::BlendState)*>(pStream));
      // NOLINTNEXTLINE
      SizeOfSubobject = sizeof(CD3DX12_PIPELINE_STATE_STREAM::BlendState);
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK: 
    // NOLINTNEXTLINE
      pCallbacks->SampleMaskCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::SampleMask)*>(pStream));
      // NOLINTNEXTLINE
      SizeOfSubobject = sizeof(CD3DX12_PIPELINE_STATE_STREAM::SampleMask);
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER: 
    // NOLINTNEXTLINE
      pCallbacks->RasterizerStateCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::RasterizerState)*>(pStream));
      // NOLINTNEXTLINE
      SizeOfSubobject = sizeof(CD3DX12_PIPELINE_STATE_STREAM::RasterizerState);
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL: 
    // NOLINTNEXTLINE
      pCallbacks->DepthStencilStateCb(*reinterpret_cast<CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL*>(pStream));
      // NOLINTNEXTLINE
      SizeOfSubobject = sizeof(CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL);
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL1: 
    // NOLINTNEXTLINE
      pCallbacks->DepthStencilState1Cb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::DepthStencilState)*>(pStream));
      // NOLINTNEXTLINE
      SizeOfSubobject = sizeof(CD3DX12_PIPELINE_STATE_STREAM::DepthStencilState);
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT: 
    // NOLINTNEXTLINE
      pCallbacks->InputLayoutCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::InputLayout)*>(pStream));
      // NOLINTNEXTLINE
      SizeOfSubobject = sizeof(CD3DX12_PIPELINE_STATE_STREAM::InputLayout);
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_IB_STRIP_CUT_VALUE: 
    // NOLINTNEXTLINE
      pCallbacks->IBStripCutValueCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::IBStripCutValue)*>(pStream));
      // NOLINTNEXTLINE
      SizeOfSubobject = sizeof(CD3DX12_PIPELINE_STATE_STREAM::IBStripCutValue);
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY: 
    // NOLINTNEXTLINE
      pCallbacks->PrimitiveTopologyTypeCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::PrimitiveTopologyType)*>(pStream));
      // NOLINTNEXTLINE
      SizeOfSubobject = sizeof(CD3DX12_PIPELINE_STATE_STREAM::PrimitiveTopologyType);
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS: 
    // NOLINTNEXTLINE
      pCallbacks->RTVFormatsCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::RTVFormats)*>(pStream));
      // NOLINTNEXTLINE
      SizeOfSubobject = sizeof(CD3DX12_PIPELINE_STATE_STREAM::RTVFormats);
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT: 
    // NOLINTNEXTLINE
      pCallbacks->DSVFormatCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::DSVFormat)*>(pStream));
      // NOLINTNEXTLINE
      SizeOfSubobject = sizeof(CD3DX12_PIPELINE_STATE_STREAM::DSVFormat);
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC: 
    // NOLINTNEXTLINE
      pCallbacks->SampleDescCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::SampleDesc)*>(pStream));
      // NOLINTNEXTLINE
      SizeOfSubobject = sizeof(CD3DX12_PIPELINE_STATE_STREAM::SampleDesc);
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_NODE_MASK: 
    // NOLINTNEXTLINE
      pCallbacks->NodeMaskCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::NodeMask)*>(pStream));
      // NOLINTNEXTLINE
      SizeOfSubobject = sizeof(CD3DX12_PIPELINE_STATE_STREAM::NodeMask);
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CACHED_PSO: 
    // NOLINTNEXTLINE
      pCallbacks->CachedPSOCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::CachedPSO)*>(pStream));
      // NOLINTNEXTLINE
      SizeOfSubobject = sizeof(CD3DX12_PIPELINE_STATE_STREAM::CachedPSO);
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_FLAGS:
    // NOLINTNEXTLINE
      pCallbacks->FlagsCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM::Flags)*>(pStream));
      // NOLINTNEXTLINE
      SizeOfSubobject = sizeof(CD3DX12_PIPELINE_STATE_STREAM::Flags);
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VIEW_INSTANCING:
    // NOLINTNEXTLINE
      pCallbacks->ViewInstancingCb(*reinterpret_cast<decltype(CD3DX12_PIPELINE_STATE_STREAM1::ViewInstancingDesc)*>(pStream));
      // NOLINTNEXTLINE
      SizeOfSubobject = sizeof(CD3DX12_PIPELINE_STATE_STREAM1::ViewInstancingDesc);
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    default:
    // NOLINTNEXTLINE
      pCallbacks->ErrorUnknownSubobject(SubobjectType);
      // NOLINTNEXTLINE
      return E_INVALIDARG;
      // NOLINTNEXTLINE
      break;
      // NOLINTNEXTLINE
    }
    // NOLINTNEXTLINE
  }
  // NOLINTNEXTLINE

// NOLINTNEXTLINE
  return S_OK;
  // NOLINTNEXTLINE
}
// NOLINTNEXTLINE

// NOLINTNEXTLINE

// NOLINTNEXTLINE
#endif // defined( __cplusplus )
// NOLINTNEXTLINE

// NOLINTNEXTLINE
#endif //__D3DX12_H__
// NOLINTNEXTLINE

// NOLINTNEXTLINE

// NOLINTNEXTLINE

// NOLINTNEXTLINE

#pragma warning( default : 4061 4324 )