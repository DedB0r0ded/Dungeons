#pragma once


#include "./backend_base.h"


namespace dungeons::backend {


	class Money {
		int value_;

		Result<void> validate_less_than_max() const;
		Result<void> validate_not_negative() const;
		Result<void> validate() const;

	public:
		Money();
		Money(int value);

		int value() const;
		Result<void> add(int value);
		Result<void> take(int value);
		bool empty() const;

		operator bool() const;
	};


}