#include "InputDebugComponent.h"
#include "Input/GameInput.h"
#include "Input/Key.h"

// ImGui はプリコンパイル済みヘッダーに含めないため直接インクルード
#include "../../ThirdParty/imgui/imgui.h"

InputDebugComponent::InputDebugComponent(ActorRef actorRef)
	: Component(actorRef)
{}

void InputDebugComponent::Setup(const EngineService& engineService)
{
	renderPipeline = engineService.Resolve<RenderPipeline>().get();
	postRenderHandle = renderPipeline->onPostRenderProcess.AddListener(std::bind(&InputDebugComponent::OnPostRender, this, std::placeholders::_1, std::placeholders::_2));
}

void InputDebugComponent::OnDestroy()
{
	if (renderPipeline && postRenderHandle != static_cast<UINT>(-1))
	{
		renderPipeline->onPostRenderProcess.RemoveListener(postRenderHandle);
	}
}

void InputDebugComponent::OnPostRender(const GraphicsContext& /*context*/, GpuConstants& /*gpuConstants*/)
{
	// ウィンドウの位置とサイズを固定しない（ドラッグ可能）
	ImGui::SetNextWindowSize(ImVec2(340.0f, 400.0f), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(10.0f, 50.0f), ImGuiCond_FirstUseEver); // 左上はカメラ座標の表示に使う

	ImGui::Begin("Input Debug");

	// =============================================
	// キーボード入力の状態表示
	// =============================================
	if (ImGui::CollapsingHeader("Keyboard", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// アルファベット
		ImGui::SeparatorText("Alphabet");
		ImGui::Indent(8.0f);

		// A〜Z をグリッド表示
		constexpr Key alphaKeys[] = {
			Key::A, Key::B, Key::C, Key::D, Key::E,
			Key::F, Key::G, Key::H, Key::I, Key::J,
			Key::K, Key::L, Key::M, Key::N, Key::O,
			Key::P, Key::Q, Key::R, Key::S, Key::T,
			Key::U, Key::V, Key::W, Key::X, Key::Y, Key::Z
		};
		constexpr char alphaLabels[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

		for (int i = 0; i < 26; ++i)
		{
			bool held = GameInput::GetKey(alphaKeys[i]);
			bool down = GameInput::GetKeyDown(alphaKeys[i]);

			// 押下中は緑、このフレームで押されたら黄色
			if (down)
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.8f, 0.1f, 1.0f));
			else if (held)
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.7f, 0.2f, 1.0f));
			else
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

			char label[3] = { alphaLabels[i], '\0' };
			ImGui::Button(label, ImVec2(24.0f, 24.0f));
			ImGui::PopStyleColor();

			if (i % 10 != 9 && i != 25)
				ImGui::SameLine(0.0f, 4.0f);
		}

		ImGui::Unindent(8.0f);

		// 数字キー
		ImGui::SeparatorText("Number");
		ImGui::Indent(8.0f);

		constexpr Key numKeys[] = {
			Key::Alpha0, Key::Alpha1, Key::Alpha2, Key::Alpha3, Key::Alpha4,
			Key::Alpha5, Key::Alpha6, Key::Alpha7, Key::Alpha8, Key::Alpha9
		};

		for (int i = 0; i < 10; ++i)
		{
			bool held = GameInput::GetKey(numKeys[i]);
			bool down = GameInput::GetKeyDown(numKeys[i]);

			if (down)
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.8f, 0.1f, 1.0f));
			else if (held)
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.7f, 0.2f, 1.0f));
			else
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

			char label[2] = { static_cast<char>('0' + i), '\0' };
			ImGui::Button(label, ImVec2(24.0f, 24.0f));
			ImGui::PopStyleColor();

			if (i != 9) ImGui::SameLine(0.0f, 4.0f);
		}

		ImGui::Unindent(8.0f);

		// 特殊キー
		ImGui::SeparatorText("Special Keys");
		ImGui::Indent(8.0f);

		// ヘルパーラムダ: ラベル付きキー状態ボタン
		auto DrawSpecialKey = [](const char* label, Key key, float width = 60.0f)
			{
				bool held = GameInput::GetKey(key);
				bool down = GameInput::GetKeyDown(key);

				if (down)
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.8f, 0.1f, 1.0f));
				else if (held)
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.7f, 0.2f, 1.0f));
				else
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

				ImGui::Button(label, ImVec2(width, 24.0f));
				ImGui::PopStyleColor();
			};

		DrawSpecialKey("Shift", Key::Shift, 60.0f); ImGui::SameLine(0.0f, 4.0f);
		DrawSpecialKey("Ctrl", Key::Control, 50.0f); ImGui::SameLine(0.0f, 4.0f);
		DrawSpecialKey("Alt", Key::Alt, 40.0f); ImGui::SameLine(0.0f, 4.0f);
		DrawSpecialKey("Space", Key::Space, 60.0f); ImGui::SameLine(0.0f, 4.0f);
		DrawSpecialKey("Enter", Key::Return, 55.0f);

		DrawSpecialKey("Backsp", Key::Backspace, 55.0f); ImGui::SameLine(0.0f, 4.0f);
		DrawSpecialKey("Tab", Key::Tab, 40.0f); ImGui::SameLine(0.0f, 4.0f);
		DrawSpecialKey("Escape", Key::Escape, 55.0f);

		// 矢印キー
		ImGui::Spacing();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 30.0f);
		DrawSpecialKey("  Up", Key::UpArrow, 32.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX());
		DrawSpecialKey("Left", Key::LeftArrow, 32.0f); ImGui::SameLine(0.0f, 4.0f);
		DrawSpecialKey("Down", Key::DownArrow, 32.0f); ImGui::SameLine(0.0f, 4.0f);
		DrawSpecialKey("Right", Key::RightArrow, 32.0f);

		ImGui::Unindent(8.0f);
	}

	// =============================================
	// マウス入力の状態表示
	// =============================================
	if (ImGui::CollapsingHeader("Mouse", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent(8.0f);

		auto DrawMouseButton = [](const char* label, MouseButton btn)
			{
				bool held = GameInput::GetMouseButton(btn);
				bool down = GameInput::GetMouseButtonDown(btn);

				if (down)
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.8f, 0.1f, 1.0f));
				else if (held)
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.7f, 0.2f, 1.0f));
				else
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

				ImGui::Button(label, ImVec2(60.0f, 24.0f));
				ImGui::PopStyleColor();
			};

		DrawMouseButton("Left", MouseButton::Left);   ImGui::SameLine(0.0f, 4.0f);
		DrawMouseButton("Right", MouseButton::Right);  ImGui::SameLine(0.0f, 4.0f);
		DrawMouseButton("Middle", MouseButton::Middle);

		ImGui::Spacing();
		ImGui::Text("  Mouse X1:"); ImGui::SameLine();
		DrawMouseButton("X1", MouseButton::X1);
		ImGui::SameLine(0.0f, 4.0f);
		ImGui::Text("  X2:"); ImGui::SameLine();
		DrawMouseButton("X2", MouseButton::X2);

		ImGui::Unindent(8.0f);
	}

	// =============================================
	// 凡例
	// =============================================
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::TextColored(ImVec4(0.1f, 0.7f, 0.2f, 1.0f), "■ Held");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.1f, 1.0f), "■ Just Pressed");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "■ Released");

	ImGui::End();
}
