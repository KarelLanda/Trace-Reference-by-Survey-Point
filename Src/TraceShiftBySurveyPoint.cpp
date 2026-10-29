#include "TraceShiftBySurveyPoint.hpp"
#include <cmath>

GSErrCode ShiftTraceBySurveyPoint()
{
	// 1. Zjištění aktuální databáze / okna
	API_DatabaseInfo dbInfo = {};
	GSErrCode err = ACAPI_Database_GetCurrentDatabase(&dbInfo);
	if (err != NoError) {
		ACAPI_WriteReport("Při zjišťování aktuálního okna došlo k chybě.", true);
		return err;
	}

	// Podporujeme pouze půdorys (APIWind_FloorPlanID) a pracovní listy (APIWind_WorksheetID)
	if (dbInfo.typeID != APIWind_FloorPlanID && dbInfo.typeID != APIWind_WorksheetID) {
		ACAPI_WriteReport("Tento příkaz lze použít pouze v půdorysu nebo v pracovním listu.", true);
		return NoError;
	}

	// 2. Získání API_DatabaseUnId pro aktuální okno
	API_DatabaseUnId parentDbId = dbInfo.databaseUnId;
	if (dbInfo.typeID == APIWind_FloorPlanID) {
		short userId = 0;
		UInt32 databaseId = 1;
		ACAPI_Database_GetGuidFromDBUnId(&userId, &databaseId, &parentDbId.elemSetId);
	}

	// 3. Načtení záznamu o průhledovém zobrazení (GhostRecord)
	API_GhostRecord ghostRecord = {};
	ghostRecord.parentDatabase = parentDbId;

	err = ACAPI_View_GetGhostRecord(&parentDbId, &ghostRecord);
	if (err == APIERR_WINDNOTEXIST || !ghostRecord.activated) {
		ACAPI_WriteReport("V aktuálním okně není aktivní žádné průhledové zobrazení.", true);
		return NoError;
	}

	if (err != NoError) {
		ACAPI_WriteReport("Nepodařilo se načíst nastavení průhledového zobrazení.", true);
		return err;
	}

	// Kontrola, zda je průhled zapnutý
	if (!ghostRecord.show) {
		ACAPI_WriteReport("Průhledové zobrazení není zapnuto.", true);
		return NoError;
	}

	// 4. Získání geodetického bodu (Survey Point) a úhlu Severu
	API_GeoLocation geoLocation = {};
	err = ACAPI_GeoLocation_GetGeoLocation(&geoLocation);
	if (err != NoError) {
		ACAPI_WriteReport("Nepodařilo se načíst nastavení geodetického bodu (Survey Point).", true);
		return err;
	}

	double rawPosX = geoLocation.surveyPointPosition.x;
	double rawPosY = geoLocation.surveyPointPosition.y;

	if (rawPosX == 0.0 && rawPosY == 0.0) {
		ACAPI_WriteReport("Geodetický bod není nastaven, posun průhledového zobrazení nebyl proveden.", true);
		return NoError;
	}

	// 5. Zjištění posunu virtuálního počátku (Far From Origin offset) pro aktivní okno (parent) a průhled (ghost)
	API_Coord parentOffset = {};
	err = ACAPI_ProjectSetting_GetOffset(&parentOffset);
	if (err != NoError) {
		parentOffset = {};
	}

	API_Coord ghostOffset = parentOffset;
	if (ghostRecord.ghostDBType != dbInfo.typeID || ghostRecord.ghostDatabase != parentDbId) {
		API_DatabaseInfo ghostDbInfo = {};
		ghostDbInfo.typeID = ghostRecord.ghostDBType;
		ghostDbInfo.databaseUnId = ghostRecord.ghostDatabase;

		GSErrCode switchErr = ACAPI_Database_ChangeCurrentDatabase(&ghostDbInfo);
		if (switchErr == NoError) {
			GSErrCode offsetErr = ACAPI_ProjectSetting_GetOffset(&ghostOffset);
			if (offsetErr != NoError) {
				ghostOffset = {};
			}
			ACAPI_Database_ChangeCurrentDatabase(&dbInfo);
		} else {
			ghostOffset = {};
		}
	}

	// 6. Výpočet cílové rotace a posunu se započtením virtuálních počátků obou databází
	// Výpočet úhlu pro orientaci na Sever: targetAngle = northAngle - (PI / 2.0)
	const double pi = 3.14159265358979323846;
	double northAngle = geoLocation.placeInfo.north;
	double targetAngle = northAngle - (pi / 2.0);

	// Bod P_world v průhledové databázi má lokální souřadnice P_local = P_world - ghostOffset.
	// V aktivním okně (parent) má bod Q_world lokální souřadnice Q_local = Q_world - parentOffset.
	// Transformace průhledu počítá: Q_local = R(targetAngle) * P_local + ghostRecord.offset.
	// Požadujeme Q_world = R(targetAngle) * P_world + S_world, odkud plyne:
	// ghostRecord.offset = S_world - parentOffset + R(targetAngle) * ghostOffset
	double cosA = std::cos(targetAngle);
	double sinA = std::sin(targetAngle);
	double rotGhostOffsetX = ghostOffset.x * cosA - ghostOffset.y * sinA;
	double rotGhostOffsetY = ghostOffset.x * sinA + ghostOffset.y * cosA;

	ghostRecord.rotation = targetAngle;
	ghostRecord.offset.x = rawPosX - parentOffset.x + rotGhostOffsetX;
	ghostRecord.offset.y = rawPosY - parentOffset.y + rotGhostOffsetY;

	// 7. Uložení upraveného průhledu do Archicadu
	err = ACAPI_View_ChangeGhostRecord(&ghostRecord);
	if (err != NoError) {
		ACAPI_WriteReport("Při změně nastavení průhledového zobrazení došlo k chybě.", true);
		return err;
	}

	// 8. Překreslení pohledu
	ACAPI_View_Redraw();

	return NoError;
}

GSErrCode MenuCommandHandler(const API_MenuParams* menuParams)
{
	if (menuParams->menuItemRef.menuResID == 32500 && menuParams->menuItemRef.itemIndex == 1) {
		return ShiftTraceBySurveyPoint();
	}
	return NoError;
}
