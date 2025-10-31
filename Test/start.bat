@echo off
setlocal EnableExtensions EnableDelayedExpansion

rem ===== ضبط المسارات =====
rem تعيين المسار الحالي للسكريبت
set "ROOT=%~dp0"
set "TESTS=%ROOT%"
set "OUT=%ROOT%results.txt"

echo Tests folder: %TESTS%
echo.

rem ===== البحث عن ملف EXE =====
set "EXE=%ROOT%FilteringRecords.exe"
if not exist "%EXE%" set "EXE=%ROOT%x64\Debug\FilteringRecords.exe"
if not exist "%EXE%" set "EXE=%ROOT%Debug\FilteringRecords.exe"
if not exist "%EXE%" set "EXE=%ROOT%x64\Release\FilteringRecords.exe"
if not exist "%EXE%" set "EXE=%ROOT%Release\FilteringRecords.exe"

if not exist "%EXE%" (
  echo [ERROR] Could not find FilteringRecords.exe
  echo Searched in:
  echo   - %ROOT%
  echo   - %ROOT%x64\Debug
  echo   - %ROOT%Debug
  echo   - %ROOT%x64\Release
  echo   - %ROOT%Release
  echo.
  echo Build the project first, then re-run.
  pause
  exit /b 1
)

echo Found EXE: %EXE%
echo.

if not exist "%TESTS%" (
  echo [ERROR] Tests folder not found: %TESTS%
  pause
  exit /b 1
)

rem حذف ملف النتائج القديم
if exist "%OUT%" del "%OUT%"

rem ======================
rem دالة تشغيل اختبار واحد
rem ======================
goto :START

:RunTest
  set "FOLD=%~1"
  set "TITLE=%~2"
  set "CASE=%TESTS%%FOLD%"
  
  if not exist "%CASE%" (
     echo [WARN] Missing test folder: %FOLD%
     echo [WARN] Missing test folder: %FOLD%>>"%OUT%"
     goto :eof
  )
  
  if not exist "%CASE%\items.txt" (
    echo [WARN] Missing items.txt in %FOLD%
    echo [WARN] Missing items.txt in %FOLD%>>"%OUT%"
  )
  
  if not exist "%CASE%\rules.txt" (
    echo [WARN] Missing rules.txt in %FOLD%
    echo [WARN] Missing rules.txt in %FOLD%>>"%OUT%"
  )
  
  echo Running: %FOLD% - %TITLE%
  echo.>>"%OUT%"
  echo ==== TEST %FOLD% ====>>"%OUT%"
  echo %TITLE%>>"%OUT%"
  echo.>>"%OUT%"
  
  rem 
  "%EXE%" "%CASE%\items.txt" "%CASE%\rules.txt" "%CASE%\output.txt"
  
  rem
  if exist "%CASE%\output.txt" (
     copy /Y "%CASE%\output.txt" "%CASE%\actual.txt" >nul 2>&1
     echo Output:>>"%OUT%"
     type "%CASE%\output.txt">>"%OUT%"
     echo.>>"%OUT%"
  ) else (
     echo [ERROR] No output produced.>>"%OUT%"
     echo.>>"%OUT%"
  )
  
goto :eof

rem ======================
rem Starting All Tests...
rem ======================
:START

echo ========================================
echo Starting All Tests...
echo ========================================
echo.

call :RunTest "01_single_match"                "Один класс совпал с одной записью"
call :RunTest "02_multi_rules_one_record"      "Одна запись подходит под несколько правил"
call :RunTest "03_no_match"                    "Нет совпадений ни с одним правилом"
call :RunTest "04_multi_records_diff_classes"  "Несколько записей → разные классы"
call :RunTest "05_class_no_matches"            "Класс без совпадений"
call :RunTest "06_all_rules_pass"              "Одна запись удовлетворяет всем правилам"
call :RunTest "07_red_blue_doors_wheels"       "Набор правил: цвет/двери/колёса"
call :RunTest "08_equals_exactly_match"        "Полное совпадение массива значений (= [...])"
call :RunTest "09_equals_exactly_no_match"     "Полное совпадение: нет совпадения"
call :RunTest "10_property_size_exact"         "property \"x\" has N values — точное N"
call :RunTest "11_contains_value_multi"        "contains value — совпадения по значениям"
call :RunTest "12_has_property_only"           "has property — наличие свойства"
call :RunTest "13_mixed_valid_and_invalid_rules" "Смешанные валидные/невалидные правила"
call :RunTest "14_many_records_perf_light"     "Производительный тест: много записей (лайт)"
call :RunTest "15_edge_empty_values"           "Пограничный случай: пустые списки значений []"

echo.
echo ========================================
echo Done! Results collected in:
echo %OUT%
echo ========================================
echo.
echo Each test's raw output also saved to tests\*\actual.txt
echo.
pause