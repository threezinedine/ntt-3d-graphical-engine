#pragma once

#define NTT_RAII(name)                                                                                                 \
public:                                                                                                                \
	name();                                                                                                            \
	name(const name&);                                                                                                 \
	name(name&&) noexcept;                                                                                             \
	~name();