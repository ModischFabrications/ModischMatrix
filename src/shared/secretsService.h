#pragma once

// pun intended. 

// CI and other pipelines need something to work with, using the template should be fine. 
#ifdef CI_BUILD
#pragma message(-- CI Build, mocking secrets --)
#include "secrets_template.h"
#else
#include "secrets.h"
#endif
