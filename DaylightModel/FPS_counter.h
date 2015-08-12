#pragma once



class FPS_counter
{
	double lastTime;
	int nbFrames;

	double frame_time;		///< Średni czas potrzebny na wyrenderowanie jedenj klatki.
public:
	FPS_counter() = default;
	~FPS_counter() = default;

	void init( double time );
#ifdef __GLFW__
	void count();
#endif
	void count( double time );
	
	inline double get_frame_time() { return frame_time; }
};

