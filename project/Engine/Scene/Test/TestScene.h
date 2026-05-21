#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */
/* engine */
#include <Engine/Extensions/Fog/FogEffect.h>
#include <Engine/scene/Base/BaseScene.h>
#include <Engine/Renderer/Sprite/Sprite.h>
#include <Engine/Objects/3D/Actor/TestObject/CalyxHuman.h>
/* c++ */
#include <memory>
#include <string>
#include <vector>

///デバッグ関連///
#ifdef _DEBUG

#include <externals/imgui/imgui.h>
#endif // _DEBUG

/* ========================================================================
/* testScene
/* ===================================================================== */
class TestScene final :
	public BaseScene{
public:
	//===================================================================*/
	//			public methods
	//===================================================================*/
	TestScene();
	~TestScene() override = default;

	void Initialize()override;
	void Update(float dt)override;
	void Draw(ID3D12GraphicsCommandList* cmdLst, class PipelineService* psoService, IRenderTarget* )override;
	void CleanUp()override;
	void LoadAssets()override;
private:
	void FetchFacultyFromWebApi();
	std::string BuildFacultyDisplayText(const std::string& response) const;
	std::string BuildFacultyEndpoint() const;
	void DrawWebApiDebugWindow();
	std::string BuildFacultyEndpoint() const;

	/* graphics =====================================================*/
	std::unique_ptr<FogEffect>fog_ = nullptr;

	/* objects ====================================================*/
	std::shared_ptr<BaseGameObject> modelField_;
	std::unique_ptr<Sprite> testSprite_;
	std::shared_ptr<CalyxHuman> animationHuman_;

	/* web api ====================================================*/
	int facultyId_ = 2;
	std::string apiStatus_ = "not requested";
	std::string apiRawResponse_;
	std::string apiDisplayText_;

};

