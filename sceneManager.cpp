#include "stdafx.h"
#include "sceneManager.h"
#include "gameNode.h"

DWORD CALLBACK loadingThread(LPVOID prc)
{
	//���� ��(ü������ �Ͼ ��)�� �����س���
	sceneManager::_readyScene->init();

	// ���� ���� �������ش�
	sceneManager::_currentScene = sceneManager::_readyScene;

	sceneManager::_loadingScene->release();
	sceneManager::_loadingScene = NULL;
	sceneManager::_readyScene = NULL;

	return 0;
}

gameNode* sceneManager::_currentScene = NULL;
gameNode* sceneManager::_loadingScene = NULL;
gameNode* sceneManager::_readyScene = NULL;

sceneManager::sceneManager()
{
}


sceneManager::~sceneManager()
{

}

HRESULT sceneManager::init(void)
{
	_currentScene = NULL;
	_loadingScene = NULL;
	_readyScene = NULL;

	return S_OK;
}

void sceneManager::release(void)
{
	mapSceneIter miSceneList = _mSceneList.begin();

	for (; miSceneList != _mSceneList.end();)
	{
		if (miSceneList->second != NULL)
		{
			if (miSceneList->second == _currentScene) miSceneList->second->release();

			SAFE_DELETE(miSceneList->second);

			miSceneList = _mSceneList.erase(miSceneList);
		}
		else ++miSceneList;
	}

	_mSceneList.clear();
}

void sceneManager::update(void)
{
	if (_currentScene) _currentScene->update();
}

void sceneManager::render(void)
{
	if (_currentScene) _currentScene->render();
}


//�� �߰��ϴ� �Լ�
gameNode* sceneManager::addScene(string sceneName, gameNode* scene)
{
	//�ش� ���� ���ٸ� �� ó��
	if (!scene) return NULL;

	//	�ʿ� �߰��Ѵ� 				  Ű��       ��
	_mSceneList.insert(make_pair(sceneName, scene));

	return scene;

}


gameNode* sceneManager::addLoadingScene(string loadingSceneName, gameNode* scene)
{
	if (!scene) return NULL;

	_mSceneList.insert(make_pair(loadingSceneName, scene));

	return scene;
}


HRESULT sceneManager::changeScene(string sceneName)
{
	//���ͷ����ͷ�        ã�����ϴ� Ű���� ���Թ޴´�
	mapSceneIter find = _mSceneList.find(sceneName);

	//���ͷ����Ͱ� ���� ���ٸ�      ���ٰ� ��ȯ
	if (find == _mSceneList.end()) return E_FAIL;

	//���ͷ����Ͱ� �����	         �ٲ��ʿ� ����
	if (find->second == _currentScene) return S_OK;

	//ã�Ҵٸ� �ش� ���� �ʱ�ȭ
	if (SUCCEEDED(find->second->init()))
	{
		if (_currentScene) _currentScene->release();

		_currentScene = find->second;

		return S_OK;
	}

	return E_FAIL;
}

HRESULT sceneManager::changeScene(string sceneName, string loadingSceneName)
{
	mapSceneIter find = _mSceneList.find(sceneName);

	if (find == _mSceneList.end()) return E_FAIL;

	if (find->second == _currentScene) return S_OK;

	mapSceneIter findLoading = _mLoadingSceneList.find(loadingSceneName);

	if (find == _mLoadingSceneList.end()) return changeScene(loadingSceneName);

	if (SUCCEEDED(find->second->init()))
	{
		if (_currentScene) _currentScene->release();

		_loadingScene = findLoading->second;

		_readyScene = find->second;

		CloseHandle(CreateThread(NULL, 0, loadingThread, NULL, 0, &_loadingThreadID));
	}

	return E_FAIL;
}

