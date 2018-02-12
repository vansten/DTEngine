#include "Matrix.h"

#include "Quaternion.h"

const Matrix Matrix::IDENTITY(1.0f, 0.0f, 0.0f, 0.0f,
							  0.0f, 1.0f, 0.0f, 0.0f,
							  0.0f, 0.0f, 1.0f, 0.0f,
							  0.0f, 0.0f, 0.0f, 1.0f);

Quaternion Matrix::ToQuaternion() const
{
	const float trace = M11 + M22 + M33;
	if(trace > 0)
	{
		const float s = 0.5f / Math::Sqrt(trace + 1.0f);
		const float w = 0.25f / s;
		const float x = (M32 - M23) * s;
		const float y = (M13 - M31) * s;
		const float z = (M21 - M12) * s;

		return Quaternion(x, y, z, w);
	}
	else
	{
		if(M11 > M22 && M11 > M33)
		{
			const float s = 2.0f * Math::Sqrt(1.0f + M11 - M22 - M33);
			const float w = (M32 - M23) / s;
			const float x = 0.25f * s;
			const float y = (M12 + M21) / s;
			const float z = (M13 + M31) / s;

			return Quaternion(x, y, z, w);
		}
		else if(M22 > M33)
		{
			const float s = 2.0f * Math::Sqrt(1.0f + M22 - M11 - M33);
			const float w = (M13 - M31) / s;
			const float x = (M12 + M21) / s;
			const float y = 0.25f * s;
			const float z = (M23 + M32) / s;

			return Quaternion(x, y, z, w);
		}
		else
		{
			const float s = 2.0f * Math::Sqrt(1.0f + M33 - M11 - M22);
			const float w = (M21 - M12) / s;
			const float x = (M13 + M31) / s;
			const float y = (M23 + M32) / s;
			const float z = 0.25f * s;

			return Quaternion(x, y, z, w);
		}
	}
}
