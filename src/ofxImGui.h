#pragma once

#include "imgui.h"
#include "Gui.h"
#include "imHelpers.h"
 
#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

namespace ImGui
{

	inline char* str2char(char* dest, const std::string& src, size_t num) {
		strncpy(dest, src.c_str(), num - 1);
		*(dest + num - 1) = '\0';
		return dest;
	}

	class ExampleAppLog
	{
	public:

		ImGuiTextBuffer     Buf;
		ImGuiTextFilter     Filter;
		ImVector<int>       LineOffsets;        // Index to lines offset
		bool                ScrollToBottom;

		void    Clear() { Buf.clear(); LineOffsets.clear(); }

		void    AddLog(const char* fmt, ...) IM_PRINTFARGS(2)
		{
			int old_size = Buf.size();
			va_list args;
			va_start(args, fmt);
			Buf.appendv(fmt, args);
			va_end(args);
			for (int new_size = Buf.size(); old_size < new_size; old_size++)
				if (Buf[old_size] == '\n')
					LineOffsets.push_back(old_size);
			ScrollToBottom = true;
		}

		void    Draw(const char* title, bool* p_open = NULL)
		{
			ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiSetCond_FirstUseEver);
			ImGui::Begin(title, p_open);
			if (ImGui::Button("Clear")) Clear();
			ImGui::SameLine();
			bool copy = ImGui::Button("Copy");
			ImGui::SameLine();
			Filter.Draw("Filter", -100.0f);
			ImGui::Separator();
			ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
			if (copy) ImGui::LogToClipboard();

			if (Filter.IsActive())
			{
				const char* buf_begin = Buf.begin();
				const char* line = buf_begin;
				for (int line_no = 0; line != NULL; line_no++)
				{
					const char* line_end = (line_no < LineOffsets.Size) ? buf_begin + LineOffsets[line_no] : NULL;
					if (Filter.PassFilter(line, line_end))
						ImGui::TextUnformatted(line, line_end);
					line = line_end && line_end[1] ? line_end + 1 : NULL;
				}
			}
			else
			{
				ImGui::TextUnformatted(Buf.begin());
			}

			if (ScrollToBottom)
				ImGui::SetScrollHere(1.0f);
			ScrollToBottom = false;
			ImGui::EndChild();
			ImGui::End();
		}
		void    Draw(const char* title, ImVec2 pos, ImVec2 size, bool* p_open = NULL)
		{
			//ウィンドウのオプション
			static bool no_titlebar = true;
			static bool no_border = false;
			static bool no_resize = true;
			static bool no_move = true;
			static bool no_scrollbar = false;
			static bool no_collapse = true;
			static bool no_menu = true;
			static bool show_another_window = false;
			static ImGuiWindowFlags window_flags = 0;

			if (no_titlebar)  window_flags |= ImGuiWindowFlags_NoTitleBar;
			if (!no_border)   window_flags |= ImGuiWindowFlags_ShowBorders;
			if (no_resize)    window_flags |= ImGuiWindowFlags_NoResize;
			if (no_move)      window_flags |= ImGuiWindowFlags_NoMove;
			if (no_scrollbar) window_flags |= ImGuiWindowFlags_NoScrollbar;
			if (no_collapse)  window_flags |= ImGuiWindowFlags_NoCollapse;
			if (!no_menu)     window_flags |= ImGuiWindowFlags_MenuBar;

			ImGui::SetNextWindowPos(pos);
			ImGui::SetNextWindowSize(size);
			ImGui::Begin(title, p_open, window_flags);
			if (ImGui::Button("Clear")) Clear();
			ImGui::SameLine();
			bool copy = ImGui::Button("Copy");
			ImGui::SameLine();
			Filter.Draw("Filter", -100.0f);
			ImGui::Separator();
			ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
			if (copy) ImGui::LogToClipboard();

			if (Filter.IsActive())
			{
				const char* buf_begin = Buf.begin();
				const char* line = buf_begin;
				for (int line_no = 0; line != NULL; line_no++)
				{
					const char* line_end = (line_no < LineOffsets.Size) ? buf_begin + LineOffsets[line_no] : NULL;
					if (Filter.PassFilter(line, line_end))
						ImGui::TextUnformatted(line, line_end);
					line = line_end && line_end[1] ? line_end + 1 : NULL;
				}
			}
			else
			{
				ImGui::TextUnformatted(Buf.begin());
			}

			if (ScrollToBottom)
				ImGui::SetScrollHere(1.0f);

			ScrollToBottom = false;
			ImGui::EndChild();
			ImGui::End();
		}
	};

	inline void setImGUIColor(int id, ImVec4 color) {
		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[id] = ImVec4(color.x / 255.f, color.y / 255.f, color.z / 255.f, color.w / 255.f);
	}

	static bool ColorPicker(float *col, bool alphabar)
	{
		const int    EDGE_SIZE = 200; // = int( ImGui::GetWindowWidth() * 0.75f );
		const ImVec2 SV_PICKER_SIZE = ImVec2(EDGE_SIZE, EDGE_SIZE);
		const float  SPACING = ImGui::GetStyle().ItemInnerSpacing.x;
		const float  HUE_PICKER_WIDTH = 20.f;
		const float  CROSSHAIR_SIZE = 7.0f;

		ImColor color(col[0], col[1], col[2]);
		bool value_changed = false;

		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		// setup

		ImVec2 picker_pos = ImGui::GetCursorScreenPos();

		float hue, saturation, value;
		ImGui::ColorConvertRGBtoHSV(
			color.Value.x, color.Value.y, color.Value.z, hue, saturation, value);

		// draw hue bar

		ImColor colors[] = { ImColor(255, 0, 0),
			ImColor(255, 255, 0),
			ImColor(0, 255, 0),
			ImColor(0, 255, 255),
			ImColor(0, 0, 255),
			ImColor(255, 0, 255),
			ImColor(255, 0, 0) };

		for (int i = 0; i < 6; ++i)
		{
			draw_list->AddRectFilledMultiColor(
				ImVec2(picker_pos.x + SV_PICKER_SIZE.x + SPACING, picker_pos.y + i * (SV_PICKER_SIZE.y / 6)),
				ImVec2(picker_pos.x + SV_PICKER_SIZE.x + SPACING + HUE_PICKER_WIDTH,
					picker_pos.y + (i + 1) * (SV_PICKER_SIZE.y / 6)),
				colors[i],
				colors[i],
				colors[i + 1],
				colors[i + 1]);
		}

		draw_list->AddLine(
			ImVec2(picker_pos.x + SV_PICKER_SIZE.x + SPACING - 2, picker_pos.y + hue * SV_PICKER_SIZE.y),
			ImVec2(picker_pos.x + SV_PICKER_SIZE.x + SPACING + 2 + HUE_PICKER_WIDTH, picker_pos.y + hue * SV_PICKER_SIZE.y),
			ImColor(255, 255, 255));

		// draw alpha bar

		if (alphabar) {
			float alpha = col[3];

			draw_list->AddRectFilledMultiColor(
				ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 2 * SPACING + HUE_PICKER_WIDTH, picker_pos.y),
				ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 2 * SPACING + 2 * HUE_PICKER_WIDTH, picker_pos.y + SV_PICKER_SIZE.y),
				ImColor(0, 0, 0), ImColor(0, 0, 0), ImColor(255, 255, 255), ImColor(255, 255, 255));

			draw_list->AddLine(
				ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 2 * (SPACING - 2) + HUE_PICKER_WIDTH, picker_pos.y + alpha * SV_PICKER_SIZE.y),
				ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 2 * (SPACING + 2) + 2 * HUE_PICKER_WIDTH, picker_pos.y + alpha * SV_PICKER_SIZE.y),
				ImColor(255.f - alpha, 255.f, 255.f));
		}

		// draw color matrix

		{
			const ImU32 c_oColorBlack = ImGui::ColorConvertFloat4ToU32(ImVec4(0.f, 0.f, 0.f, 1.f));
			const ImU32 c_oColorBlackTransparent = ImGui::ColorConvertFloat4ToU32(ImVec4(0.f, 0.f, 0.f, 0.f));
			const ImU32 c_oColorWhite = ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 1.f, 1.f, 1.f));

			ImVec4 cHueValue(1, 1, 1, 1);
			ImGui::ColorConvertHSVtoRGB(hue, 1, 1, cHueValue.x, cHueValue.y, cHueValue.z);
			ImU32 oHueColor = ImGui::ColorConvertFloat4ToU32(cHueValue);

			draw_list->AddRectFilledMultiColor(
				ImVec2(picker_pos.x, picker_pos.y),
				ImVec2(picker_pos.x + SV_PICKER_SIZE.x, picker_pos.y + SV_PICKER_SIZE.y),
				c_oColorWhite,
				oHueColor,
				oHueColor,
				c_oColorWhite
			);

			draw_list->AddRectFilledMultiColor(
				ImVec2(picker_pos.x, picker_pos.y),
				ImVec2(picker_pos.x + SV_PICKER_SIZE.x, picker_pos.y + SV_PICKER_SIZE.y),
				c_oColorBlackTransparent,
				c_oColorBlackTransparent,
				c_oColorBlack,
				c_oColorBlack
			);
		}

		// draw cross-hair

		float x = saturation * SV_PICKER_SIZE.x;
		float y = (1 - value) * SV_PICKER_SIZE.y;
		ImVec2 p(picker_pos.x + x, picker_pos.y + y);
		draw_list->AddLine(ImVec2(p.x - CROSSHAIR_SIZE, p.y), ImVec2(p.x - 2, p.y), ImColor(255, 255, 255));
		draw_list->AddLine(ImVec2(p.x + CROSSHAIR_SIZE, p.y), ImVec2(p.x + 2, p.y), ImColor(255, 255, 255));
		draw_list->AddLine(ImVec2(p.x, p.y + CROSSHAIR_SIZE), ImVec2(p.x, p.y + 2), ImColor(255, 255, 255));
		draw_list->AddLine(ImVec2(p.x, p.y - CROSSHAIR_SIZE), ImVec2(p.x, p.y - 2), ImColor(255, 255, 255));

		// color matrix logic

		ImGui::InvisibleButton("saturation_value_selector", SV_PICKER_SIZE);

		if (ImGui::IsItemActive() && ImGui::GetIO().MouseDown[0])
		{
			ImVec2 mouse_pos_in_canvas = ImVec2(
				ImGui::GetIO().MousePos.x - picker_pos.x, ImGui::GetIO().MousePos.y - picker_pos.y);

			/**/ if (mouse_pos_in_canvas.x <                     0) mouse_pos_in_canvas.x = 0;
			else if (mouse_pos_in_canvas.x >= SV_PICKER_SIZE.x - 1) mouse_pos_in_canvas.x = SV_PICKER_SIZE.x - 1;

			/**/ if (mouse_pos_in_canvas.y <                     0) mouse_pos_in_canvas.y = 0;
			else if (mouse_pos_in_canvas.y >= SV_PICKER_SIZE.y - 1) mouse_pos_in_canvas.y = SV_PICKER_SIZE.y - 1;

			value = 1 - (mouse_pos_in_canvas.y / (SV_PICKER_SIZE.y - 1));
			saturation = mouse_pos_in_canvas.x / (SV_PICKER_SIZE.x - 1);
			value_changed = true;
		}

		// hue bar logic

		ImGui::SetCursorScreenPos(ImVec2(picker_pos.x + SPACING + SV_PICKER_SIZE.x, picker_pos.y));
		ImGui::InvisibleButton("hue_selector", ImVec2(HUE_PICKER_WIDTH, SV_PICKER_SIZE.y));

		if (ImGui::GetIO().MouseDown[0] && (ImGui::IsItemHovered() || ImGui::IsItemActive()))
		{
			ImVec2 mouse_pos_in_canvas = ImVec2(
				ImGui::GetIO().MousePos.x - picker_pos.x, ImGui::GetIO().MousePos.y - picker_pos.y);

			/**/ if (mouse_pos_in_canvas.y <                     0) mouse_pos_in_canvas.y = 0;
			else if (mouse_pos_in_canvas.y >= SV_PICKER_SIZE.y - 1) mouse_pos_in_canvas.y = SV_PICKER_SIZE.y - 1;

			hue = mouse_pos_in_canvas.y / (SV_PICKER_SIZE.y - 1);
			value_changed = true;
		}

		// alpha bar logic

		if (alphabar) {

			ImGui::SetCursorScreenPos(ImVec2(picker_pos.x + SPACING * 2 + HUE_PICKER_WIDTH + SV_PICKER_SIZE.x, picker_pos.y));
			ImGui::InvisibleButton("alpha_selector", ImVec2(HUE_PICKER_WIDTH, SV_PICKER_SIZE.y));

			if (ImGui::GetIO().MouseDown[0] && (ImGui::IsItemHovered() || ImGui::IsItemActive()))
			{
				ImVec2 mouse_pos_in_canvas = ImVec2(
					ImGui::GetIO().MousePos.x - picker_pos.x, ImGui::GetIO().MousePos.y - picker_pos.y);

				/**/ if (mouse_pos_in_canvas.y <                     0) mouse_pos_in_canvas.y = 0;
				else if (mouse_pos_in_canvas.y >= SV_PICKER_SIZE.y - 1) mouse_pos_in_canvas.y = SV_PICKER_SIZE.y - 1;

				float alpha = mouse_pos_in_canvas.y / (SV_PICKER_SIZE.y - 1);
				col[3] = alpha;
				value_changed = true;
			}

		}

		// R,G,B or H,S,V color editor

		color = ImColor::HSV(hue >= 1 ? hue - 10 * 1e-6 : hue, saturation > 0 ? saturation : 10 * 1e-6, value > 0 ? value : 1e-6);
		col[0] = color.Value.x;
		col[1] = color.Value.y;
		col[2] = color.Value.z;

		bool widget_used;
		ImGui::PushItemWidth((alphabar ? SPACING + HUE_PICKER_WIDTH : 0) +
			SV_PICKER_SIZE.x + SPACING + HUE_PICKER_WIDTH - 2 * ImGui::GetStyle().FramePadding.x);
		widget_used = alphabar ? ImGui::ColorEdit4("", col) : ImGui::ColorEdit3("", col);
		ImGui::PopItemWidth();

		// try to cancel hue wrap (after ColorEdit), if any
		{
			float new_hue, new_sat, new_val;
			ImGui::ColorConvertRGBtoHSV(col[0], col[1], col[2], new_hue, new_sat, new_val);
			if (new_hue <= 0 && hue > 0) {
				if (new_val <= 0 && value != new_val) {
					color = ImColor::HSV(hue, saturation, new_val <= 0 ? value * 0.5f : new_val);
					col[0] = color.Value.x;
					col[1] = color.Value.y;
					col[2] = color.Value.z;
				}
				else
					if (new_sat <= 0) {
						color = ImColor::HSV(hue, new_sat <= 0 ? saturation * 0.5f : new_sat, new_val);
						col[0] = color.Value.x;
						col[1] = color.Value.y;
						col[2] = color.Value.z;
					}
			}
		}

		return value_changed | widget_used;
	}

#ifdef __OPENCV_OLD_CV_H__
	static void ConvertMatToGL(const cv::Mat& src, GLuint* texID) {
		if (src.empty() == true)
			return;

		glDeleteTextures(1, texID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, texID);
		glBindTexture(GL_TEXTURE_2D, *texID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
		if (src.channels() == 4) {
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGBA,
				src.cols, src.rows,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				src.ptr<unsigned char>()
			);
		}
		else if (src.channels() == 3) {
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGB,
				src.cols, src.rows,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				src.ptr<unsigned char>()
			);
		}
		else if (src.channels() == 1) {
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_LUMINANCE,
				src.cols, src.rows,
				0,
				GL_LUMINANCE,
				GL_UNSIGNED_BYTE,
				src.ptr<unsigned char>()
			);
		}
		else {
			ofLog() << "other channel !";
		}
        
        
}
#endif
}
