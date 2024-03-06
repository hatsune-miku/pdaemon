module;

#include <Windows.h>
#include <winnls.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>
#include <shobjidl.h>

export module file_utils;

import <format>;
import <stdexcept>;

import byte_utils;

export tstring basename(const tstring& path) {
	auto pos = path.find_last_of(TEXT("/\\"));
	if (pos == tstring::npos) {
		return path;
	}
	return path.substr(pos + 1);
}

export tstring path_of(const tstring& path) {
	auto pos = path.find_last_of(TEXT("/\\"));
	if (pos == tstring::npos) {
		return TEXT("");
	}
	return path.substr(0, pos);
}

export tstring path_join(const tstring& part0, const tstring& part1) {
	if (part0.empty()) {
		return part1;
	}
	if (part1.empty()) {
		return part0;
	}
	if (part0.back() == TEXT('\\') || part0.back() == TEXT('/')) {
		return part0 + part1;
	}
	return part0 + TEXT("\\") + part1;
}

export bool is_lnk(const tstring& path) noexcept {
	return path.size() >= 4 && path.substr(path.size() - 4) == TEXT(".lnk");
}

/**
* @brief Follows the link and returns the target path. 
* If the path is not a link, returns the path itself.
*/
export tstring follow_lnk(const tstring& path) {
	if (!is_lnk(path)) {
		return path;
	}

	tstring ret;
	HRESULT hr = CoInitialize(NULL);

	if (!SUCCEEDED(hr)) {
		throw std::exception("CoInitialize failed");
	}

	IShellLink* psl;
	hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, reinterpret_cast<void**>(&psl));
	if (FAILED(hr)) {
		CoUninitialize();
		throw std::exception("CoCreateInstance failed");
	}

	IPersistFile* ppf;
	hr = psl->QueryInterface(IID_IPersistFile, reinterpret_cast<void**>(&ppf));
	if (FAILED(hr)) {
		psl->Release();
		CoUninitialize();
		throw std::exception("QueryInterface failed");
	}

	hr = ppf->Load(path.c_str(), STGM_READ);
	if (SUCCEEDED(hr)) {
		WCHAR szTarget[MAX_PATH];
		if (SUCCEEDED(psl->GetPath(szTarget, MAX_PATH, NULL, SLGP_UNCPRIORITY))) {
			ret.assign(szTarget);
		}
	}

	ppf->Release();
	psl->Release();
	CoUninitialize();
	return ret;
}

export void make_link(const tstring& path, const tstring& target) {
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)) {
		throw std::exception("CoInitialize failed");
	}

	IShellLink* pShellLink = NULL;
	IPersistFile* pPersistFile;

	hr = CoCreateInstance(
		CLSID_ShellLink,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IShellLink,
		reinterpret_cast<LPVOID*>(&pShellLink)
	);
	if (FAILED(hr)) {
		CoUninitialize();
		throw std::exception("CoCreateInstance failed");
	}

	tstring arguments = std::format(TEXT("\"{}\""), target);
	tstring lnk_filename = std::format(TEXT("protect_{}.lnk"), basename(target));
	tstring target_base_path = path_of(target);
	tstring base_path = path_of(path);
	tstring lnk_path = path_join(base_path, lnk_filename);

	pShellLink->SetPath(path.c_str());
	pShellLink->SetWorkingDirectory(target_base_path.c_str());
	pShellLink->SetArguments(arguments.c_str());

	hr = pShellLink->QueryInterface(
		IID_IPersistFile,
		reinterpret_cast<void**>(&pPersistFile)
	);

	if (SUCCEEDED(hr)) {
		hr = pPersistFile->Save(lnk_path.c_str(), TRUE);
		pPersistFile->Release();
	}

	pShellLink->Release();
	CoUninitialize();
}
