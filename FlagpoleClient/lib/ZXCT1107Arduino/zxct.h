#include "Arduino.h"

class ZXCT1107{
	public:
	
		ZXCT1107(uint8_t pin);
		
		bool begin();
	
		virtual float read_voltage();
		float read_current();
        float read_salinity();
        
    protected:

	private:
		
};

#endif