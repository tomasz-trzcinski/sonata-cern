#ifndef __H_ENUMS__
#define __H_ENUMS__

namespace Render
{
	enum RenderType
	{
		TEST,
		RENDER_A,
		RENDER_B,
		RENDER_C,
		RENDER_D
	};

	enum CalcType
	{
		GPU,
		CPU
	};

	enum LineType
	{
		GEOM,
		HOBBY
	};

	enum RecordType
	{
		DYNAMIC,
		STATIC
	};

	enum ThreadType
	{
		SINGLE,
		MULTI
	};
};

#endif