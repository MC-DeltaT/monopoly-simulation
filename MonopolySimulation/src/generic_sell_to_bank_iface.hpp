#pragma once


namespace monopoly {

	enum class generic_sell_to_bank_type {
		street,
		railway,
		utility,
		building
	};

	struct generic_sell_to_bank_t {
		generic_sell_to_bank_type type;
		// Street -> street index. Railway -> railway index. Utility -> utility index. Building -> street index.
		unsigned data1;
		// Building -> how many to sell. Unused for other types.
		unsigned data2 = 0;
	};

}
