#include "Scoreloop.h"
#include "Trace.h"

#ifdef ENABLE_SCORELOOP

SC_Client_h gScoreloopClient;

//---------------------------------------------------------------------------------------------------------------------
void InitScoreloop()
{
  TRACE_FILE_IF(1) TRACE("Scoreloop client creation");

  // My game
  const char* gameID = "f0db0734-2209-4bde-9719-c90c7346ba5d";
  const char* gameSecret = "BxgwSOy61SJSYouzqJo6ZYEd7Fjo+sSpa4qOZ3qQFGYJ1fUrIXRoBA==";

  // My test game
  //const char* gameID = "dd89abad-8584-4014-960e-b24a463ef120";
  //const char* gameSecret = "HoIZN+FsTUmnFmKvF5ytzraeI7yBXOCOyOq5eWiUsD3/y9ExXgpGog==";

  // Demo
  //const char* gameID = "b9b41ecc-8d98-4363-bfb3-c530e3452456";
  //const char* gameSecret = "9RqaCfv3bAYhxyviEipsmxmOh3dbApWlrpdmY1hlCUXOxa5wAE19wg==";

  const char* currency = "GSA";
  const char* languageCode = "en";
  const char* gameVersion = "1.0";

  SC_Error_t errCode;
  SC_ClientConfig_h clientConf;
  // create an instance of the SC_ClientConfig class
  errCode = SC_ClientConfig_New(&clientConf);
  // aGameId is the unique game identifier (const char string that you obtain from Scoreloop).
  SC_ClientConfig_SetGameIdentifier(clientConf, gameID);
  // aGameSecret is the unique game secret (const char string that you obtain from Scoreloop).
  SC_ClientConfig_SetGameSecret(clientConf,gameSecret);
  // aCurrency is the unique game currency code (const char string that you obtain from Scoreloop).
  SC_ClientConfig_SetGameCurrency(clientConf, currency);
  // aGameVersion should be your current game version.
  SC_ClientConfig_SetGameVersion(clientConf, gameVersion);
  // aLanguageCode specifies the language support for localization in awards.
  // Its a comma-separated list of languages the game supports. e.g., for English, pass "en"
  SC_ClientConfig_SetLanguages(clientConf, languageCode);

  errCode = SC_Client_NewWithConfig(&gScoreloopClient, clientConf);

  SC_ClientConfig_Delete(clientConf);

  TRACE_FILE_IF(1) TRACE("Scoreloop client result = %d", errCode);
}

void TermScoreloop()
{
  TRACE_FILE_IF(1) TRACE("Scoreloop terminate");
  SC_Client_Release(gScoreloopClient);
}

#else

SC_Client_h gScoreloopClient = 0;

void InitScoreloop()
{
  TRACE_FILE_IF(1) TRACE("Scoreloop (null) init");
}


void TermScoreloop()
{
  TRACE_FILE_IF(1) TRACE("Scoreloop (null) terminate");
}
#endif