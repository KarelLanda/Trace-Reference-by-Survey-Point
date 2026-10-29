# PruhledoveZobrazeniDleGB (Archicad Add-On)

Doplněk pro Archicad 29+, který automaticky zarovná a otočí aktuální průhledové zobrazení (Trace & Reference) podle Geodetického bodu (Survey Point) a úhlu Severu se započtením virtuálních počátků obou databází.

## Parametry doplňku
- **Název doplňku:** `PruhledoveZobrazeniDleGB`
- **Příkaz v menu:** `Hotline Cegra` -> `Posun Odkazu dle GB`
- **Developer ID:** `944205126`
- **Local ID:** `323968475`
- **Podporovaná okna:** Půdorysy (`APIWind_FloorPlanID`) a Pracovní listy (`APIWind_WorksheetID`)

## Princip fungování
1. Ověří, zda je v aktuálním okně (půdorys nebo pracovní list) aktivní a zapnuté průhledové zobrazení (`API_GhostRecord`).
2. Načte fyzické souřadnice Geodetického bodu v souřadném systému projektu Archicadu (`surveyPointPosition.x`, `surveyPointPosition.y`).
3. Načte úhel Severu a spočte cílové otočení k Severu:
   $$\text{targetAngle} = \text{northAngle} - \frac{\pi}{2}$$
4. Načte virtuální posun počátku aktivního okna (`parentOffset`) a průhledového zobrazení (`ghostOffset`) pomocí `ACAPI_ProjectSetting_GetOffset`, čímž plně eliminuje fenomén přepočtu souřadnic daleko od počátku (Far From Origin) po uložení a znovuotevření projektu.
5. Vypočte a nastaví přesný posun a rotaci průhledu:
   $$\text{ghostRecord.offset} = S_{\text{world}} - O_{\text{parent}} + R(\text{targetAngle}) \cdot O_{\text{ghost}}$$
6. Uloží změny přes `ACAPI_View_ChangeGhostRecord` a překreslí okno (`ACAPI_View_Redraw`).

## Sestavení (macOS)
```bash
cmake -B Build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build Build --config Release
```
Po úspěšném sestavení se `.bundle` automaticky zkopíruje do testovací složky `Testovane doplnky/`.

## Sestavení (Windows)
Sestavení probíhá automaticky přes GitHub Actions workflow (`.github/workflows/build-and-release.yml`), který po pushnutí vygeneruje Windows balíček `.apx`.
