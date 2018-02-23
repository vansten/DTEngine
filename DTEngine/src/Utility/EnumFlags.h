#pragma once

template<typename Enum>
struct EnumFlags
{
public:
	Enum Flags;

public:
	constexpr EnumFlags() : Flags((Enum)0)
	{}
	constexpr EnumFlags(Enum flag) : Flags(flag)
	{}
	constexpr EnumFlags(const EnumFlags<Enum>& flags) : Flags(flags.Flags)
	{}

	constexpr inline void RaiseFlag(Enum flag)
	{
		Flags = (Enum)((int)Flags | (int)flag);
	}

	constexpr inline void RaiseFlags(EnumFlags<Enum> flags)
	{
		Flags = (Enum)((int)Flags | (int)flags.Flags);
	}

	constexpr inline void ClearFlag(Enum flag)
	{
		Flags = (Enum)((int)Flags & ~((int)flag));
	}

	constexpr inline void ClearFlags(EnumFlags<Enum> flags)
	{
		Flags = (Enum)((int)Flags & ~((int)flags.Flags));
	}

	constexpr inline void ClearAllFlags()
	{
		Flags = (Enum)0;
	}

	constexpr inline bool IsFlagSet(Enum flag) const
	{
		return (Flags & flag) != 0;
	}

	constexpr inline bool AreFlagsSet(EnumFlags<Enum> flags) const
	{
		return (Flags & flags.Flags) != 0;
	}

	constexpr inline bool IsAnyFlagSet() const
	{
		return Flags != (Enum)0;
	}

	constexpr inline operator bool() const
	{
		return IsAnyFlagSet();
	}

	constexpr inline void operator=(const EnumFlags<Enum>& other)
	{
		Flags = other.Flags;
	}

	constexpr inline void operator=(Enum e)
	{
		Flags = e;
	}

	constexpr inline bool operator==(const EnumFlags<Enum>& other)
	{
		return Flags == other.Flags;
	}

	constexpr inline bool operator!=(const EnumFlags<Enum>& other)
	{
		return Flags != other.Flags;
	}
};

template<typename Enum>
constexpr inline EnumFlags<Enum> operator|(Enum e1, Enum e2)
{
	int flags = (int)e1 | (int)e2;
	return EnumFlags<Enum>((Enum)(flags));
}

template<typename Enum>
constexpr inline EnumFlags<Enum> operator|(EnumFlags<Enum> e1, EnumFlags<Enum> e2)
{
	int flags = (int)e1.Flags | (int)e2.Flags;
	return EnumFlags<Enum>((Enum)(flags));
}

template<typename Enum>
constexpr inline EnumFlags<Enum> operator|(EnumFlags<Enum> e1, Enum e2)
{
	int flags = (int)e1.Flags | (int)e2;
	return EnumFlags<Enum>((Enum)(flags));
}

template<typename Enum>
constexpr inline EnumFlags<Enum> operator|(Enum e1, EnumFlags<Enum> e2)
{
	int flags = (int)e1 | (int)e2.Flags;
	return EnumFlags<Enum>((Enum)(flags));
}

template<typename Enum>
constexpr inline EnumFlags<Enum> operator&(Enum e1, Enum e2)
{
	int flags = (int)e1 & (int)e2;
	return EnumFlags<Enum>((Enum)(flags));
}

template<typename Enum>
constexpr inline EnumFlags<Enum> operator&(EnumFlags<Enum> e1, Enum e2)
{
	int flags = (int)e1.Flags & (int)e2;
	return EnumFlags<Enum>((Enum)(flags));
}

template<typename Enum>
constexpr inline EnumFlags<Enum> operator&(Enum e1, EnumFlags<Enum> e2)
{
	int flags = (int)e1 & (int)e2.Flags;
	return EnumFlags<Enum>((Enum)(flags));
}

template<typename Enum>
constexpr inline EnumFlags<Enum> operator&(EnumFlags<Enum> e1, EnumFlags<Enum> e2)
{
	int flags = (int)e1.Flags & (int)e2.Flags;
	return EnumFlags<Enum>((Enum)(flags));
}
