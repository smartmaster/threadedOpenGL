#pragma once
#include "SmlAxisCoord.h"
#include <vector>
#include <random>
#include <string>
#include <iostream>
#include <cassert>
#include <QDebug>
#include <glm/gtx/string_cast.hpp>


namespace SmartLib
{

	using namespace ::std;

	class AxisCoordTest
	{
	private:
		template<typename ITER>
		static void FillRandom(ITER begin, ITER end, remove_cvref_t<decltype(*begin)> scalar, long long minval, long long maxval, long long invalidVal)
		{
			random_device randDevice;
			mt19937_64 randEngine(randDevice());
			uniform_int_distribution<long long> randDistrib(minval, maxval);

			while (begin != end)
			{
				long long randVal = randDistrib(randEngine);
				if (randVal != invalidVal)
				{
					*begin = randVal * scalar;
					++begin;
				}
			}
		}

		template<typename T>
		static bool CompareMat4(const glm::tmat4x4<T>& mat1, const glm::tmat4x4<T>& mat2, T const eps)
		{
			auto str1 = glm::to_string(mat1);
			auto str2 = glm::to_string(mat2);

			qDebug() << str1.c_str() << Qt::endl;
			qDebug() << str2.c_str() << Qt::endl << Qt::endl;

			bool ok = true;
			for (int ii = 0; ii < mat1.length(); ++ii)
			{
				auto bv4 = glm::epsilonEqual(mat1[ii], mat2[ii], eps);
				ok = glm::all(bv4);
				assert(ok);
				if (!ok)
				{
					break;
				}
			}
			return ok;
		}

	public:
		static void Case0_projection()
		{
			using T = double;

			const size_t dataSize = 10000;
			vector<T> data(dataSize);
			FillRandom(data.begin(), data.end(), 0.001, -99999, 99999, 0);

			const int loopCount = 20000;
			int index = 0;
			for (int ii = 0; ii < loopCount; ++ii)
			{
				T left = data[index % dataSize]; ++index;
				T right = data[index % dataSize]; ++index;
				T bottom = data[index % dataSize]; ++index;
				T top = data[index % dataSize]; ++index;
				T znear = data[index % dataSize]; ++index;
				T zfar = data[index % dataSize]; ++index;

				{
					auto mat = AxisCoord<T>::Ortho(left, right, bottom, top, znear, zfar);
					auto matGlm = glm::ortho(left, right, bottom, top, znear, zfar);
					const T eps = 1e-5;
					assert(CompareMat4(mat, matGlm, eps));
				}
				{
					auto mat = AxisCoord<T>::Frustum(left, right, bottom, top, znear, zfar);
					auto matGlm = glm::frustum(left, right, bottom, top, znear, zfar);
					const T eps = 1e-5;
					assert(CompareMat4(mat, matGlm, eps));
				}

			}
		}
	};
}