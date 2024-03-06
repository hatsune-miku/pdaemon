module;

#include <Windows.h>
#include <taskschd.h>
#include <comdef.h>

#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")

export module taskschd_utils;

import file_utils;
import byte_utils;
import <string>;

export bool is_task_exist(const tstring& task_name) {
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	bool ret = false;

	ITaskService* pService{};
	ITaskFolder* pRootFolder{};
	IRegisteredTask* pRegisteredTask{};

	HRESULT hr = CoCreateInstance(
		CLSID_TaskScheduler,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_ITaskService,
		reinterpret_cast<LPVOID*>(&pService)
	);

	if (SUCCEEDED(hr)) {
		// Connecting using default parameters.
		hr = pService->Connect(
			_variant_t(),
			_variant_t(),
			_variant_t(),
			_variant_t()
		);
	}

	if (SUCCEEDED(hr)) {
		hr = pService->GetFolder(_bstr_t(TEXT("\\")), &pRootFolder);
	}

	if (SUCCEEDED(hr)) {
		hr = pRootFolder->GetTask(_bstr_t(task_name.c_str()), &pRegisteredTask);
	}

	if (SUCCEEDED(hr)) {
		ret = true;
		pRegisteredTask->Release();
	}
	else {
		ret = false;
	}

	if (pRootFolder) {
		pRootFolder->Release();
	}

	if (pService) {
		pService->Release();
	}

	CoUninitialize();

	return ret;
}

export HRESULT create_login_task(
	const tstring& task_name,
	const tstring& author,
	const tstring& description,
	const tstring& path,
	const tstring& working_directory,
	const tstring& arguments
) {
	HRESULT hr = S_OK;

	if (is_task_exist(task_name)) {
		return hr;
	}

	// 热不热闹啊
	ITaskService* pService{};
	ITaskDefinition* pTask{};
	IRegistrationInfo* pRegInfo{};
	IPrincipal* pPrincipal{};
	ITriggerCollection* pTriggerCollection{};
	ITrigger* pTrigger{};
	ITaskSettings* pSettings{};
	ILogonTrigger* pLogonTrigger{};
	IActionCollection* pActionCollection{};
	IAction* pAction{};
	IExecAction* pExecAction{};
	ITaskFolder* pRootFolder{};
	IRegisteredTask* pRegisteredTask{};

	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr)) {
		return hr;
	}

	hr = CoCreateInstance(
		CLSID_TaskScheduler,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_ITaskService,
		reinterpret_cast<LPVOID*>(&pService)
	);
	if (FAILED(hr)) {
		CoUninitialize();
		return hr;
	}

	hr = pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
	if (FAILED(hr)) {
		pService->Release();
		CoUninitialize();
		return hr;
	}

	hr = pService->NewTask(0, &pTask);

	if (FAILED(hr)) {
		CoUninitialize();
		return hr;
	}

	hr = pTask->get_RegistrationInfo(&pRegInfo);
	if (SUCCEEDED(hr)) {
		pRegInfo->put_Author(bstr_t(author.c_str()));
		pRegInfo->put_Description(bstr_t(description.c_str()));

		// 这是什么神奇的设置值的方式啊
		// 我喜欢
		pRegInfo->Release();
	}

	hr = pTask->get_Settings(&pSettings);
	if (SUCCEEDED(hr)) {
		pSettings->put_StartWhenAvailable(VARIANT_TRUE);
		pSettings->put_DisallowStartIfOnBatteries(VARIANT_FALSE);
		pSettings->put_StopIfGoingOnBatteries(VARIANT_FALSE);
		pSettings->put_ExecutionTimeLimit(_bstr_t(TEXT("PT0S")));
		pSettings->Release();
	}

	hr = pTask->get_Principal(&pPrincipal);
	if (SUCCEEDED(hr)) {
		pPrincipal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);
		pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);
		pPrincipal->Release();
	}

	hr = pTask->get_Triggers(&pTriggerCollection);
	if (FAILED(hr)) {
		pTask->Release();
		CoUninitialize();
		return hr;
	}

	hr = pTriggerCollection->Create(TASK_TRIGGER_LOGON, &pTrigger);
	pTriggerCollection->Release();
	if (FAILED(hr)) {
		pTask->Release();
		CoUninitialize();
		return hr;
	}

	hr = pTrigger->QueryInterface(IID_ILogonTrigger, reinterpret_cast<LPVOID*>(&pLogonTrigger));
	if (FAILED(hr)) {
		pTask->Release();
		CoUninitialize();
		return hr;
	}

	hr = pLogonTrigger->put_Id(_bstr_t(TEXT("LogonTrigger")));
	if (FAILED(hr)) {
		pTask->Release();
		CoUninitialize();
		return hr;
	}

	hr = pTask->get_Actions(&pActionCollection);
	if (FAILED(hr)) {
		pTask->Release();
		CoUninitialize();
		return hr;
	}

	hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
	pActionCollection->Release();
	if (FAILED(hr)) {
		pTask->Release();
		CoUninitialize();
		return hr;
	}

	hr = pAction->QueryInterface(IID_IExecAction, reinterpret_cast<LPVOID*>(&pExecAction));
	pAction->Release();
	if (FAILED(hr)) {
		pTask->Release();
		CoUninitialize();
		return hr;
	}

	pExecAction->put_Path(_bstr_t(path.c_str()));
	pExecAction->put_WorkingDirectory(_bstr_t(working_directory.c_str()));
	pExecAction->put_Arguments(_bstr_t(arguments.c_str()));
	pExecAction->Release();

	// Register the task!
	hr = pService->GetFolder(_bstr_t(TEXT("\\")), reinterpret_cast<ITaskFolder**>(&pRootFolder));
	if (FAILED(hr)) {
		pTask->Release();
		CoUninitialize();
		return hr;
	}

	hr = pRootFolder->RegisterTaskDefinition(
		_bstr_t(task_name.c_str()),
		pTask,
		TASK_CREATE_OR_UPDATE,
		/* Account not specified */ _variant_t(),
		/* Password not specified */ _variant_t(),
		TASK_LOGON_INTERACTIVE_TOKEN,
		_variant_t(TEXT("")),
		&pRegisteredTask
	);

	if (SUCCEEDED(hr)) {
		pRegisteredTask->Release();
	}

	pService->Release();
	pRootFolder->Release();
	pTask->Release();
	CoUninitialize();

	return hr;
}
