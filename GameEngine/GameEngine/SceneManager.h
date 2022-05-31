#pragma once

/// 씬들을 관리하는 매니저
/// 

class SceneBase;


class SceneManager
{
public:
   SceneManager();
   ~SceneManager();

   void UpdateNowScene(float dTime);
 
   void AddScene(std::string name, SceneBase* scene);
   void SetNowScene(std::string name);
   void SetNowScene(unsigned int sceneNumber);
   SceneBase* GetNowScene();
   const std::string& GetNowSceneName();

   const unsigned int GetSceneCnt();
   SceneBase* GetSceneByIndex(unsigned int sceneNumber);
   const std::string GetSceneUMKey(unsigned int sceneNumber);

private:
	std::unordered_map<std::string, SceneBase*> m_Scenes_UM;
   SceneBase* m_pNowScene;
   SceneBase* m_pPrevScene;

};

