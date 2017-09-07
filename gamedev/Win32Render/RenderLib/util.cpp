#include "stdafx.h"
#include "util.h"

std::wstring vec3_to_wstr(Vec3f const &v)
{
	std::wstring str = L" ", num = std::to_wstring(v.x);
	str.append(num, 0, num.length() / 2);
	str += L"    ";

	num = std::to_wstring(v.y);
	str.append(num, 0, num.length() / 2);
	str += L"    ";

	num = std::to_wstring(v.z);
	str.append(num, 0, num.length() / 2);

	return str;
}