#ifndef MOTIONVALUES_H
#define MOTIONVALUES_H

#include "../math/math3d.h"
#include "../Mobitech.h"
#include <vector>

class MotionValues
{
public:
	enum Motion 
    {
		M_LINE,
		M_SLINE,
		M_DISCONTINUOUS
	};

private:

	Motion _type;

public:

	struct TimedKey 
    {
		float time;
		float value;
		float gradient;
	};

	vector<TimedKey> keys;
	
    void Clear() { keys.clear(); }
	float AddKey(float time, float value)
	{
		TimedKey key;
		key.time = time;
		key.value = value;
		keys.push_back(key);
		return time;
	}

	int Value(float t, float &localT)
	{
		if (keys.front().time > t || t > keys.back().time) {
			return -1;
		}
		size_t start = 0;
		size_t end = keys.size() - 2;
		size_t middle = (start + end) / 2;
		while (!(keys[middle].time <= t && t <= keys[middle + 1].time)) {
			if (keys[middle].time > t) {
				end = middle;
			} else if (keys[middle + 1].time < t) {
				start = middle + 1;
			}
			middle = (start + end) / 2;
		}

		localT = (t - keys[middle].time) / (keys[middle + 1].time - keys[middle].time);
		return middle;
	}

	void SetType(Motion type)
	{		
		if (keys.size() == 1) 
        {
			AddKey(1.f, keys[0].value);
		};
		_type = type;
		if (_type == M_SLINE) 
        {
			float g1, g2, g3;
			if (keys.size() > 1) 
            {
				keys[0].gradient = keys[1].value - keys[0].value;
				keys[keys.size() - 1].gradient = keys[keys.size() - 1].value - keys[keys.size() - 2].value;
			}

			for (size_t i = 1; i < (keys.size()-1); i++)
            {
				g1 = keys[i].value - keys[i - 1].value;
				g2 = keys[i + 1].value - keys[i].value;
				g3 = g2 - g1;
				keys[i].gradient = g1 + 0.5f * g3;
			}
		}
	}

	float GetFrame(size_t i, float t)
	{
		if (_type == M_LINE) 
			return lerp(keys[i].value, keys[i+1].value, t);
		if (_type == M_DISCONTINUOUS) 
			return keys[i].value;
		return spline_lerp(keys[i].value, keys[i+1].value, keys[i].gradient, keys[i+1].gradient, t);
	}

};

#endif//MOTIONVALUES_H