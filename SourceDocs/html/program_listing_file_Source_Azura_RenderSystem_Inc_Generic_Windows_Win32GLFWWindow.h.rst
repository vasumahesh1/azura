
.. _program_listing_file_Source_Azura_RenderSystem_Inc_Generic_Windows_Win32GLFWWindow.h:

Program Listing for File Win32GLFWWindow.h
==========================================

|exhale_lsh| :ref:`Return to documentation for file <file_Source_Azura_RenderSystem_Inc_Generic_Windows_Win32GLFWWindow.h>` (``Source\Azura\RenderSystem\Inc\Generic\Windows\Win32GLFWWindow.h``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   #include "Generic/GLFWWindow.h"
   #include <windows.h>
   
   namespace Azura {
   class Win32GLFWWindow final : public GLFWWindow {
    public:
     Win32GLFWWindow(const String& title, U32 width, U32 height);
     const void* GetHandle() const override;
   
     bool Initialize() override;
   private:
     HWND m_win32Window;
   };
   }  // namespace Azura
