#ifndef SCORELOOP_H
#define SCORELOOP_H

#define ENABLE_SCORELOOPx

void InitScoreloop();
void TermScoreloop();

#ifdef ENABLE_SCORELOOP

#include "scoreloop/sc_client.h"

#else

typedef int SC_Error_t;
typedef int SC_ScoreController_h;
typedef int SC_Score_h;
typedef int SC_User_h;
typedef int SC_UserController_h;
typedef int SC_ScoresController_h;
typedef int SC_RequestControllerCompletionCallback_t;
typedef int SC_Client_h;

typedef int SC_Session_h;

enum SC_Error_tag {
    SC_OK                 = 0,
};

inline void SC_UserController_Release(SC_UserController_h self) {}

inline SC_Error_t SC_ScoreController_SubmitScore(SC_ScoreController_h self, SC_Score_h score) {return SC_OK;}
inline SC_Error_t SC_Score_SetResult(SC_Score_h self, double result) {return SC_OK;}
inline SC_Error_t SC_Score_SetMinorResult(SC_Score_h self, double minorResult) {return SC_OK;}
inline SC_Error_t SC_Score_SetMode(SC_Score_h self, unsigned int mode) {return SC_OK;}
template<typename T>
inline SC_Error_t SC_Client_CreateScoreController(SC_Client_h self,
        SC_ScoreController_h* pScoreController,
        T callback, void* cookie)  {return SC_OK;}
inline SC_Error_t SC_Client_CreateScore(SC_Client_h self, SC_Score_h *pScore)  {return SC_OK;}
inline void SC_ScoreController_Release(SC_ScoreController_h self) {}
inline void SC_Score_Release(SC_Score_h self) {}
inline SC_Error_t SC_ScoreController_Cancel(SC_ScoreController_h self)  {return SC_OK;}

#endif

extern SC_Client_h gScoreloopClient;



#endif

