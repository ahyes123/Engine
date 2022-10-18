#pragma once

class TextFactory;
class Text;

class DebugInformation
{
public:
	static void ShowFPSText();
private:
	static std::shared_ptr<Text> myFpsText;
	static unsigned short myRealFPS;
	static unsigned long myRealFPSAvarage;
};