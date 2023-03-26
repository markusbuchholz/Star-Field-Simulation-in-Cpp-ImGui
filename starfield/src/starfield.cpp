// Markus Buchholz, 2023

#include <stdafx.hpp>
#include "imgui_helper.hpp"
#include <tuple>
#include <vector>
#include <math.h>
#include <random>
#include <algorithm>
#include <thread>
#include <chrono>
#include <Eigen/Dense>

//---------------------------------------------------------------

int w = 1000;
int h = 1000;
float center_x = w / 2.0;
float center_y = h / 2.0;
int starsN = 200;

//---------------------------------------------------------------
float randomInit()
{
	std::random_device device;
	std::mt19937 engine(device());
	// std::uniform_real_distribution<float> dist(0, (float)w);
	std::uniform_real_distribution<float> dist(0.0f, w);
	return dist(engine);
}

//---------------------------------------------------------------
Eigen::Matrix<float, 3, 3> RotZ(float ang)
{

	float c = std::cos((M_PI / 180) * ang);
	float s = std::sin((M_PI / 180) * ang);

	Eigen::Matrix<float, 3, 3> Rz;
	Rz << c, -s, 0.0f,
		s, c, 0.0f,
		0.0f, 0.0f, 0.0f;
	return Rz;
}

//---------------------------------------------------------------

struct Star
{
	float x;
	float y;
	float z;
	float r;

	Star()
	{

		this->x = randomInit();
		this->y = randomInit();
		this->z = 0.0f;
		this->r = 1.0f;
	}

	void move()
	{

		this->z = this->z + 1.0f;
		if (this->y > h || this->x > w || this->y < 0 || this->x < 0)
		{
			this->x = randomInit();
			this->y = randomInit();
			this->z = 0.0f;
			this->r = 1.0f;
		}

		else
		{
			this->x = (this->x - (float)w / 2.0f) * 1.05 + (float)w / 2.0f;
			this->y = (this->y - (float)h / 2.0f) * 1.05 + (float)h / 2.0f;
			this->r = this->r + 0.1f;
		}
	}

	void rotate(float ang)
	{

		Eigen::Matrix<float, 3, 1> C;
		C << center_x, center_y, 0.0f;

		Eigen::Matrix<float, 3, 1> P;
		P << this->x, this->y, 0.0f;

		Eigen::Matrix<float, 3, 1> Pr;

		Pr = RotZ(ang) * (P - C) + C;
		this->x = Pr(0, 0);
		this->y = Pr(1, 0);
	}
};

//---------------------------------------------------------------

int main(int argc, char const *argv[])
{

	std::string title = "Starfield simulation";
	initImgui(w, h, title);

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.0f / 255.0, 0.0f / 0.0, 0.0f / 255.0, 1.00f);
	ImVec4 line_color = ImVec4(255.0f / 255.0, 255.0f / 255.0, 255.0f / 255.0, 1.00f);
	ImVec4 blue_color = ImVec4(0.0f / 255.0, 0.0f / 255.0, 170.0f / 255.0, 1.00f);
	ImVec4 pink_color = ImVec4(179.0f / 255.0, 12.0f / 255.0, 130.0f / 255.0, 1.00f);
	ImVec4 green_color = ImVec4(60.0f / 255.0, 160.0f / 255.0, 90.0f / 255.0, 1.00f);
	ImVec4 dot_color = ImVec4(255.0f / 255.0, 255.0f / 255.0, 255.0f / 255.0, 1.00f);

	std::vector<Star> stars;

	// Star *star = new Star[starsN];

	for (int ii = 0; ii < starsN; ii++)
	{
		stars.emplace_back(Star());
	}

	while (!glfwWindowShouldClose(window) && flag == true)
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();

		ImGuiWindowFlags window_flags = 0;
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_FirstUseEver);
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoBackground;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGui::Begin("Simulation", nullptr, window_flags);
		ImDrawList *draw_list = ImGui::GetWindowDrawList();
		for (auto &s : stars)
		{
			s.move();
			draw_list->AddCircleFilled({s.x, s.y}, s.r, ImColor(dot_color));
		}

		for (auto &s : stars)
		{
			s.rotate(1.0f);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		ImGui::End();

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	termImgui();
	return 0;
}
