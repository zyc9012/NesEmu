#include "Config.h"
#include "../utils.h"

bool ParseCommandLine(Config& config, int argc, char** argv)
{
	bool romSpecified = false;
	for (int i = 0; i < argc;)
	{
		if (strcmp("-rom", argv[i]) == 0) {
			i++;
			config.RomFile = string(argv[i]);
			romSpecified = true;
		}
		else if (strcmp("-scale", argv[i]) == 0) {
			i++;
			config.Scale = (float)atof(argv[i]);
		}
		else if (strcmp("-filter", argv[i]) == 0) {
			i++;
			if (strcmp("nearest", argv[i]) == 0) config.FilterMode = GL_NEAREST;
			else if (strcmp("linear", argv[i]) == 0) config.FilterMode = GL_LINEAR;
			else log("Unknown filter mode, using \"nearest\"");
		}
		i++;
	}

	if (!romSpecified)
	{
		log("Please specify rom file with -rom");
		return false;
	}

	config.TimePerFrame = 1.0 / config.TargetFps;

	return true;
}