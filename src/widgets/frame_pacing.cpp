/**
 * This file is part of Special K.
 *
 * Special K is free software : you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * as published by The Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * Special K is distributed in the hope that it will be useful,
 *
 * But WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Special K.
 *
 *   If not, see <http://www.gnu.org/licenses/>.
 *
**/
#include <SpecialK/widgets/widget.h>
#include <SpecialK/parameter.h>
#include <SpecialK/config.h>

extern iSK_INI* osd_ini;

extern void SK_ImGui_DrawGraph_FramePacing (void);

class SKWG_FramePacing : public SK_Widget
{
public:
  SKWG_FramePacing (void) : SK_Widget ("FramePacing")
  {
    setResizable    (                false).setAutoFit      (true).setMovable (false).
    setDockingPoint (DockAnchor::SouthEast).setClickThrough (true);

    SK_ImGui_Widgets.frame_pacing = this;
  };

  void run (void)
  {
    ImGuiIO& io (ImGui::GetIO ());

    const  float font_size           =             ImGui::GetFont  ()->FontSize                        * io.FontGlobalScale;
    const  float font_size_multiline = font_size + ImGui::GetStyle ().ItemSpacing.y + ImGui::GetStyle ().ItemInnerSpacing.y;

    ImVec2 new_size (font_size * 35, font_size_multiline * 5.44f);
    setSize (new_size);

    if (isVisible ())
      ImGui::SetNextWindowSize (new_size, ImGuiSetCond_Always);
  }

  void draw (void)
  {
    static bool move = true;
    
    if (move)
    {
      ImGuiIO& io (ImGui::GetIO ());

      const  float font_size           =             ImGui::GetFont  ()->FontSize                        * io.FontGlobalScale;
      const  float font_size_multiline = font_size + ImGui::GetStyle ().ItemSpacing.y + ImGui::GetStyle ().ItemInnerSpacing.y;

      ImGui::SetWindowPos (ImVec2 (io.DisplaySize.x - getSize ().x, io.DisplaySize.y - getSize ().y ));
      move = false;
    }

    SK_ImGui_DrawGraph_FramePacing ();
  }

  void OnConfig (ConfigEvent event)
  {
    switch (event)
    {
      case SK_Widget::ConfigEvent::LoadComplete:
        break;

      case SK_Widget::ConfigEvent::SaveStart:
        break;
    }
  }
} __frame_pacing__;