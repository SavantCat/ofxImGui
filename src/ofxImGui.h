#pragma once

#include "imgui.h"
#include "Gui.h"
#include "Helpers.h"

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

inline char* str2char(char* dest, const std::string& src, size_t num) {
	strncpy(dest, src.c_str(), num - 1);
	*(dest + num - 1) = '\0';
	return dest;
}

struct ExampleAppLog
{
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
};

inline void setImGUIColor(int id, ImVec4 color) {
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[id] = ImVec4(color.x / 255.f, color.y / 255.f, color.z / 255.f, color.w / 255.f);
}

#ifdef OPENCV_CORE_HPP
inline void ConvertMatToGL(const cv::Mat& src, GLuint* texID) {
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
		ofLog() << "other chanel";
	}
}
#endif // OPENCV_CORE_HPP

