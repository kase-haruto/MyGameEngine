

/////////////////////////////////////////////////////////////////////////////////////////
//	include
/////////////////////////////////////////////////////////////////////////////////////////

// scene
#include <Engine/Scene/Test/TestScene.h>

// engine
#include <Engine/Collision/CollisionManager.h>
#include <Engine/Foundation/Utility/Func/MyFunc.h>
#include <Engine/Scene/Serializer/SceneSerializer.h>
// lib
#include <curl/curl.h>
#include <externals/nlohmann/json.hpp>

#if defined(_DEBUG) || defined(DEVELOP)
#include <externals/imgui/imgui.h>
#endif

// c++
#include <exception>
#include <sstream>
#include <string>

namespace {
	size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userData) {
		const size_t totalSize = size * nmemb;
		auto* response = static_cast<std::string*>(userData);
		response->append(static_cast<char*>(contents), totalSize);
		return totalSize;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//	コンストラクタ/デストラクタ
/////////////////////////////////////////////////////////////////////////////////////////
TestScene::TestScene(){
	// シーン名を設定
	BaseScene::SetSceneName("TestScene");

}

/////////////////////////////////////////////////////////////////////////////////////////
//	アセットのロード
/////////////////////////////////////////////////////////////////////////////////////////
void TestScene::LoadAssets(){}

/////////////////////////////////////////////////////////////////////////////////////////
//	初期化処理
/////////////////////////////////////////////////////////////////////////////////////////
void TestScene::Initialize(){
	sceneContext_->Initialize();

	sceneContext_->SetSceneName("TestScene");

	BaseScene::Initialize();

	SceneSerializer::Load(*sceneContext_, "Resources/Assets/Scenes/TestScene.scene");

	LoadAssets();

	FetchFacultyFromWebApi();

	//=========================
	// グラフィック関連
	//=========================
}

/////////////////////////////////////////////////////////////////////////////////////////
//	更新処理
/////////////////////////////////////////////////////////////////////////////////////////
void TestScene::Update([[maybe_unused]]float dt){
	DrawWebApiDebugWindow();

	//衝突判定
	CollisionManager::GetInstance()->UpdateCollisionAllCollider();
}

void TestScene::Draw(ID3D12GraphicsCommandList* cmdList, PipelineService* psoService, IRenderTarget*  rt){

	//========================================================//
	//	spriteの登録
	//========================================================//
	// 
	// 
	//シーン上のオブジェクトの描画
	BaseScene::Draw(cmdList, psoService, rt);

}

void TestScene::CleanUp(){
	// 3Dオブジェクトの描画を終了
	sceneContext_->GetObjectLibrary()->Clear();
	CollisionManager::GetInstance()->ClearColliders();
}

void TestScene::FetchFacultyFromWebApi() {
	apiRawResponse_.clear();
	apiDisplayText_.clear();
	apiStatus_ = "requesting...";
	const std::string endpoint = BuildFacultyEndpoint();

	CURLcode globalResult = curl_global_init(CURL_GLOBAL_DEFAULT);
	if(globalResult != CURLE_OK) {
		apiStatus_ = std::string("curl_global_init failed: ") + curl_easy_strerror(globalResult);
		return;
	}

	CURL* curl = curl_easy_init();
	if(!curl) {
		apiStatus_ = "curl_easy_init failed";
		curl_global_cleanup();
		return;
	}

	curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &apiRawResponse_);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2L);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

	CURLcode result = curl_easy_perform(curl);

	long responseCode = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
	curl_easy_cleanup(curl);
	curl_global_cleanup();

	if(result != CURLE_OK) {
		apiStatus_ = std::string("request failed: ") + curl_easy_strerror(result);
		return;
	}

	if(responseCode < 200 || responseCode >= 300) {
		apiStatus_ = "HTTP error: " + std::to_string(responseCode);
		apiDisplayText_ = apiRawResponse_;
		return;
	}

	apiStatus_ = "success HTTP " + std::to_string(responseCode);
	apiDisplayText_ = BuildFacultyDisplayText(apiRawResponse_);
}

std::string TestScene::BuildFacultyDisplayText(const std::string& response) const {
	try {
		nlohmann::json data = nlohmann::json::parse(response);
		if(data.is_array()) {
			if(data.empty()) {
				return "response array is empty";
			}
			data = data.front();
		}

		if(!data.is_object()) {
			return data.dump(2);
		}

		std::ostringstream text;
		for(const auto& [key, value] : data.items()) {
			text << key << ": ";
			if(value.is_string()) {
				text << value.get<std::string>();
			} else {
				text << value.dump();
			}
			text << '\n';
		}
		return text.str();
	} catch(const std::exception& e) {
		return std::string("JSON parse failed: ") + e.what() + "\n\nRaw response:\n" + response;
	}
}

std::string TestScene::BuildFacultyEndpoint() const {
	return "http://localhost:3000/faculties/" + std::to_string(facultyId_);
}

void TestScene::DrawWebApiDebugWindow() {
#if defined(_DEBUG) || defined(DEVELOP)
	if(ImGui::Begin("WebApi Faculty")) {
		std::string endpoint = BuildFacultyEndpoint();
		ImGui::Text("Endpoint: %s", endpoint.c_str());
		ImGui::InputInt("Faculty ID", &facultyId_);
		if(facultyId_ < 1) {
			facultyId_ = 1;
		}
		ImGui::SameLine();
		if(ImGui::Button("Fetch")) {
			FetchFacultyFromWebApi();
		}
		ImGui::Text("Status: %s", apiStatus_.c_str());
		if(ImGui::Button("Reload")) {
			FetchFacultyFromWebApi();
		}
		ImGui::Separator();
		ImGui::TextWrapped("%s", apiDisplayText_.empty() ? "(no data)" : apiDisplayText_.c_str());
	}
	ImGui::End();
#endif
}
