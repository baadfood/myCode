#ifndef Display_h_
#define Display_h_

#include <string>
#include <SDL.h>

class CameraBase;
struct GameState;

class Display
{
public:
	Display(int p_width, int p_height, std::string const & p_title);
	virtual ~Display();

  void clear(float p_red, float p_green, float p_blue, float p_alpha);
	void update();
	bool isClosed();
  Uint32 getWinId();
  CameraBase * getCamera();
  void setCamera(CameraBase * p_camera);

  bool handleEvent(SDL_Event * p_event, GameState * p_state);

private:
	struct Private;
	Private * d;

	Display(Display const & p_other) {}
	Display & operator=(Display const & p_other) {}
};

#endif