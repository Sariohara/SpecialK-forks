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
#define NOMINMAX

#include <SpecialK/widgets/widget.h>
#include <SpecialK/gpu_monitor.h>
#include <SpecialK/dxgi_backend.h>

#include <algorithm>

extern iSK_INI* osd_ini;

extern std::wstring
SK_CountToString (uint64_t count);

class SKWG_D3D11_Pipeline : public SK_Widget
{
public:
  SKWG_D3D11_Pipeline (void) : SK_Widget ("D3D11_Pipeline")
  {
    SK_ImGui_Widgets.d3d11_pipeline = this;

    setAutoFit (true).setDockingPoint (DockAnchor::West).setClickThrough (true);
  };

  void run (void)
  {
    DWORD dwNow = timeGetTime ();

    if (last_update < dwNow - update_freq)
    {
      D3D11_QUERY_DATA_PIPELINE_STATISTICS& stats =
        SK::DXGI::pipeline_stats_d3d11.last_results;

      pipeline.raster.fill_ratio.addValue          ( 100.0F * static_cast <float> (stats.CPrimitives) /
                                                              static_cast <float> (stats.CInvocations),  false );
      pipeline.raster.triangles_submitted.addValue (          static_cast <float> (stats.CInvocations),  FALSE );
      pipeline.raster.pixels_filled.addValue       (          static_cast <float> (stats.PSInvocations), FALSE );
      pipeline.raster.triangles_filled.addValue    (          static_cast <float> (stats.CPrimitives),   FALSE );


      pipeline.tessellation.hull.addValue          (static_cast <float> (stats.HSInvocations), false);
      pipeline.tessellation.domain.addValue        (static_cast <float> (stats.DSInvocations), false);

      pipeline.vertex.verts_input.addValue         (static_cast <float> (stats.IAVertices),    false);
      pipeline.vertex.prims_input.addValue         (static_cast <float> (stats.IAPrimitives),  false);
      pipeline.vertex.verts_invoked.addValue       (static_cast <float> (stats.VSInvocations), false);

      pipeline.vertex.gs_invokeed.addValue         (static_cast <float> (stats.GSInvocations), false);
      pipeline.vertex.gs_output.addValue           (static_cast <float> (stats.GSPrimitives),  false);

      pipeline.compute.dispatches.addValue         (static_cast <float> (stats.CSInvocations), false);

      last_update = dwNow;
    }
  }

  void draw (void)
  {
    ImGuiIO& io (ImGui::GetIO ( ));

    const  float font_size           =             ImGui::GetFont  ()->FontSize                        * io.FontGlobalScale;
    const  float font_size_multiline = font_size + ImGui::GetStyle ().ItemSpacing.y + ImGui::GetStyle ().ItemInnerSpacing.y;

    char szAvg  [512] = { };

    if (pipeline.vertex.verts_invoked.getAvg () > 0)
    {
      static float max_invoke = (pipeline.vertex.verts_invoked.getMax ());
                   max_invoke = std::max (max_invoke, pipeline.vertex.verts_invoked.getMax ());

      sprintf_s
        ( szAvg,
            512,
              u8"Vertex Invocations:\n\n"
              u8"          min: %ws Invocations,   max: %ws Invocations,   avg: %ws Invocations\n",
                  SK_CountToString (pipeline.vertex.verts_invoked.getMin   ()).c_str (), SK_CountToString (max_invoke).c_str (),
                    SK_CountToString (pipeline.vertex.verts_invoked.getAvg ()).c_str () );

      float samples = 
        std::min ( (float)pipeline.vertex.verts_invoked.getUpdates  (),
                   (float)pipeline.vertex.verts_invoked.getCapacity () );

      ImGui::PushStyleColor ( ImGuiCol_PlotLines, 
                                ImColor::HSV ( 0.31f - 0.31f *
                         std::min ( 1.0f, pipeline.vertex.verts_invoked.getMax () / max_invoke ),
                                                 0.73f,
                                                   0.93f ) );

      ImGui::PlotLines ( "###Vtx_Assembly",
                         pipeline.vertex.verts_invoked.getValues ().data (),
        static_cast <int> (samples),
                             pipeline.vertex.verts_invoked.getOffset (),
                               szAvg,
                                 pipeline.vertex.verts_invoked.getMin   (),
                                   max_invoke,
                                     ImVec2 (
                                       ImGui::GetContentRegionAvailWidth (), font_size * 4.0f) );

      static float max_verts = (pipeline.vertex.verts_input.getMax ());
                   max_verts = std::max (max_verts, pipeline.vertex.verts_input.getMax ());

      sprintf_s
        ( szAvg,
            512,
              u8"Vertices Input:\n\n"
              u8"          min: %ws Vertices,   max: %ws Vertices,   avg: %ws Vertices\n",
                  SK_CountToString (pipeline.vertex.verts_input.getMin ()).c_str (), SK_CountToString (max_verts).c_str (),
                    SK_CountToString (pipeline.vertex.verts_input.getAvg ()).c_str () );

      samples = 
        std::min ( (float)pipeline.vertex.verts_input.getUpdates  (),
                   (float)pipeline.vertex.verts_input.getCapacity () );

      ImGui::PushStyleColor ( ImGuiCol_PlotLines, 
                                ImColor::HSV ( 0.31f - 0.31f *
                         std::min ( 1.0f, pipeline.vertex.verts_input.getMax () / max_verts ),
                                                 0.73f,
                                                   0.93f ) );

      ImGui::PlotLines ( "###Vtx_Assembly",
                         pipeline.vertex.verts_input.getValues ().data (),
        static_cast <int> (samples),
                             pipeline.vertex.verts_input.getOffset (),
                               szAvg,
                                 pipeline.vertex.verts_input.getMin   (),
                                   max_verts,
                                     ImVec2 (
                                       ImGui::GetContentRegionAvailWidth (), font_size * 4.0f) );

      static float max_prims = (pipeline.vertex.prims_input.getMax ());
                   max_prims = std::max (max_prims, pipeline.vertex.prims_input.getMax ());

      sprintf_s
        ( szAvg,
            512,
              u8"Prims Assembled:\n\n"
              u8"          min: %ws Triangles,   max: %ws Triangles,   avg: %ws Triangles\n",
                  SK_CountToString (pipeline.vertex.prims_input.getMin ()).c_str (), SK_CountToString (max_prims).c_str (),
                    SK_CountToString (pipeline.vertex.prims_input.getAvg ()).c_str () );

      samples = 
        std::min ( (float)pipeline.vertex.prims_input.getUpdates  (),
                   (float)pipeline.vertex.prims_input.getCapacity () );

      ImGui::PushStyleColor ( ImGuiCol_PlotLines, 
                                ImColor::HSV ( 0.31f - 0.31f *
                         std::min ( 1.0f, pipeline.vertex.prims_input.getMax () / max_prims ),
                                                 0.73f,
                                                   0.93f ) );

      ImGui::PlotLines ( "###Prim_Assembly",
                         pipeline.vertex.prims_input.getValues ().data (),
        static_cast <int> (samples),
                             pipeline.vertex.prims_input.getOffset (),
                               szAvg,
                                 pipeline.vertex.prims_input.getMin   (),
                                   max_prims,
                                     ImVec2 (
                                       ImGui::GetContentRegionAvailWidth (), font_size * 4.0f) );

      ImGui::PopStyleColor (3);
    }

    if (pipeline.raster.triangles_submitted.getAvg ( ) > 0)
    {
      //_swprintf ( wszDesc,
      //             L"%s  RASTER : %5.1f%% Filled     (%s Triangles IN )",
      //               wszDesc, 100.0f *
      //                   ( (float)stats.CPrimitives /
      //                     (float)stats.CInvocations ),
      //                 SK_CountToString (stats.CInvocations).c_str () );

      sprintf_s
        ( szAvg,
            512,
              u8"Raster Fill Rate:\n\n"
              u8"          min: %5.1f%%,   max: %5.1f%%,   avg: %5.1f%%\n",
                pipeline.raster.fill_ratio.getMin   (), pipeline.raster.fill_ratio.getMax (),
                  pipeline.raster.fill_ratio.getAvg () );

      float samples = 
        std::min ( (float)pipeline.raster.fill_ratio.getUpdates  (),
                   (float)pipeline.raster.fill_ratio.getCapacity () );

      static float max_ratio = (pipeline.raster.fill_ratio.getMax ());
                   max_ratio = std::max (max_ratio, pipeline.raster.fill_ratio.getMax ());

      ImGui::PushStyleColor ( ImGuiCol_PlotLines, 
                                ImColor::HSV ( 0.31f - 0.31f *
                         std::min ( 1.0f, pipeline.raster.fill_ratio.getAvg () / 100.0f ),
                                                 0.73f,
                                                   0.93f ) );

      ImGui::PlotLines ( "###Raster_Rate",
                         pipeline.raster.fill_ratio.getValues ().data (),
        static_cast <int> (samples),
                             pipeline.raster.fill_ratio.getOffset (),
                               szAvg,
                                 pipeline.raster.fill_ratio.getMin   (),
                                   max_ratio,
                                     ImVec2 (
                                       ImGui::GetContentRegionAvailWidth (), font_size * 4.0f) );

      static float max_fill = (pipeline.raster.pixels_filled.getMax ());
                   max_fill = std::max (max_fill, pipeline.raster.pixels_filled.getMax ());


      sprintf_s
        ( szAvg,
            512,
              u8"Pixels Filled:\n\n"
              u8"          min: %ws Pixels,   max: %ws Pixels,   avg: %ws Pixels\n",
                SK_CountToString (pipeline.raster.pixels_filled.getMin ()).c_str (), SK_CountToString (max_fill).c_str (),
                  SK_CountToString (pipeline.raster.pixels_filled.getAvg ()).c_str () );

      samples = 
        std::min ( (float)pipeline.raster.pixels_filled.getUpdates  (),
                   (float)pipeline.raster.pixels_filled.getCapacity () );

      ImGui::PushStyleColor ( ImGuiCol_PlotLines, 
                                ImColor::HSV ( 0.31f - 0.31f *
                         std::min ( 1.0f, pipeline.raster.pixels_filled.getMax () / max_fill ),
                                                 0.73f,
                                                   0.93f ) );

      ImGui::PlotLines ( "###Pixels_Filled",
                         pipeline.raster.pixels_filled.getValues ().data (),
        static_cast <int> (samples),
                             pipeline.raster.pixels_filled.getOffset (),
                               szAvg,
                                 pipeline.raster.pixels_filled.getMin   (),
                                   max_fill,
                                     ImVec2 (
                                       ImGui::GetContentRegionAvailWidth (), font_size * 4.0f) );

      ImGui::PopStyleColor (2);
    }
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

protected:
  const DWORD update_freq = 4UL;

private:
  DWORD last_update = 0UL;

  struct {
    struct {
      SK_ImGui_DataHistory <float, 600> verts_invoked;
      SK_ImGui_DataHistory <float, 600> verts_input;
      SK_ImGui_DataHistory <float, 600> prims_input;
      SK_ImGui_DataHistory <float, 600> gs_invokeed;
      SK_ImGui_DataHistory <float, 600> gs_output;
    } vertex;

    struct {
      SK_ImGui_DataHistory <float, 600> hull;
      SK_ImGui_DataHistory <float, 600> domain;
    } tessellation;

    struct {
      SK_ImGui_DataHistory <float, 600> fill_ratio;
      SK_ImGui_DataHistory <float, 600> triangles_submitted;
      SK_ImGui_DataHistory <float, 600> triangles_filled;
      SK_ImGui_DataHistory <float, 600> pixels_filled;
    } raster;

    struct
    {
      SK_ImGui_DataHistory <float, 600> dispatches;
    } compute;
  } pipeline;
} __d3d11_pipeline__;

