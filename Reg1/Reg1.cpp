#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <string>
#include <intrin.h>
#include <atlstr.h>
#include <codecvt>

bool fl = false;
HKEY globalKey = NULL;
void search(HKEY hKey, std::string name)
{
    DWORD    cSubKeys = 0;
    DWORD    cValues = 0;

    DWORD i, retCode;
    if (hKey != NULL)
    {
        if (RegQueryInfoKey(hKey, NULL, NULL, NULL, &cSubKeys, NULL, NULL, &cValues, NULL, NULL, NULL, NULL) == ERROR_SUCCESS && !fl)
        {
            for (i = 0; i < cSubKeys; i++)
            {
                TCHAR    achKey[16383];
                DWORD    cbName;
                cbName = 32767;
                retCode = RegEnumKeyEx(hKey, i, achKey, &cbName, NULL, NULL, NULL, NULL);
                HKEY newKey = NULL;
                if (retCode == ERROR_SUCCESS)
                {
                    int lResult = RegOpenKeyEx(hKey, achKey, 0, KEY_ALL_ACCESS, &newKey);
                    std::wstring wsTmp(name.begin(), name.end());
                    if (achKey == wsTmp)
                    {
                        fl = true;
                        globalKey = newKey;
                    }
                    else
                    {
                        search(newKey, name);
                        RegCloseKey(newKey);
                    }
                }
            }
        }
    }
}

void printKeysValues(HKEY hKey)
{
    DWORD    cSubKeys = 0;
    DWORD    cValues = 0;

    DWORD i, retCode;
    if (hKey != NULL)
    {

        retCode = RegQueryInfoKey(hKey, NULL, NULL, NULL, &cSubKeys, NULL, NULL, &cValues, NULL, NULL, NULL, NULL);

        if (cSubKeys)
        {
            for (i = 0; i < cSubKeys; i++)
            {
                TCHAR    achKey[16383];
                DWORD    cbName;
                cbName = 32767;
                retCode = RegEnumKeyEx(hKey, i, achKey, &cbName, NULL, NULL, NULL, NULL);
                if (retCode == ERROR_SUCCESS)
                {
                    std::cout << "subkey: ";
                    std::wcout << achKey;
                    std::cout << '\n';
                }
            }
        }

        if (cValues)
        {
            for (i = 0; i < cValues; i++)
            {
                DWORD cbValue;
                cbValue = 255;
                TCHAR  achValue[255];
                retCode = RegEnumValue(hKey, i, achValue, &cbValue, NULL, NULL, NULL, NULL);
                if (retCode == ERROR_SUCCESS)
                {
                    std::cout << "value: ";
                    std::wcout << achValue;
                    std::cout << '\n';
                }
            }
        }
        //OPEN[HKEY_LOCAL_MACHINE][SOFTWARE\Adobe\DefaultLanguage\CS6]
    }
}

int main()
{
    std::string command;
    std::cout << "Enter the command:\n";
    std::cout << "ROOT[root]\n";
    std::cout << "OPEN[subkey]\n";
    std::cout << "RETURN\n";
    std::cout << "CREATE[name]\n";
    std::cout << "ADD [param_name][param_type][data]\n";
    std::cout << "SEARCH[subkey]\n";
    std::cout << "FLAGREAD[path]\n";
    std::cout << "INFORM\n";
    std::cout << "EXIT\n";

    std::cin >> command;
    HKEY hKey = NULL;
    HKEY roots[30];
    int rooti = 0;
    while (command.compare("EXIT") != 0)
    {

        if (strstr(command.c_str(), "ROOT"))
        {
            std::string root = "";
            int k = 0;
            while (command.c_str()[k] != '[')
                k++;
            k++;
            while (command.c_str()[k] != ']')
            {
                root = root + command.c_str()[k];
                k++;
            }
            if (root.compare("HKEY_CLASSES_ROOT") == 0)
                hKey = HKEY_CLASSES_ROOT;
            else if (root.compare("HKEY_CURRENT_USER") == 0)
                hKey = HKEY_CURRENT_USER;
            else if (root.compare("HKEY_LOCAL_MACHINE") == 0)
                hKey = HKEY_LOCAL_MACHINE;
            else if (root.compare("HKEY_USERS") == 0)
                hKey = HKEY_USERS;
            else if (root.compare("HKEY_CURRENT_CONFIG") == 0)
                hKey = HKEY_CURRENT_CONFIG;
            printKeysValues(hKey);
            roots[rooti] = hKey;
        }
        else if (strstr(command.c_str(), "OPEN"))
        {
            std::string subkey = "";
            int k = 0;
            while (command.c_str()[k] != '[')
                k++;
            k++;
            while (command.c_str()[k] != ']')
            {
                subkey = subkey + command.c_str()[k];
                k++;
            }
           
            if (hKey != NULL)
            {
                wchar_t wtext[200];
                mbstowcs(wtext, subkey.c_str(), strlen(subkey.c_str())+1);
                LPWSTR ptr = wtext;
                rooti++;
                roots[rooti] = hKey;
                int lResult = RegOpenKeyEx(roots[rooti], ptr, 0, KEY_ALL_ACCESS, &hKey);
                if (lResult != ERROR_SUCCESS)
                {
                    if (lResult == ERROR_FILE_NOT_FOUND) {
                        printf("Key not found.\n");
                        return TRUE;
                    }
                    else {
                        printf("Error opening key.\n");
                        return FALSE;
                    }
                }
                printKeysValues(hKey);
            }

        }

        else if (strstr(command.c_str(), "RETURN"))
        {
            if (roots[rooti] != NULL && rooti>0)
            {
                RegCloseKey(hKey);
                hKey = roots[rooti];
                rooti--;
                printKeysValues(hKey);
            }
        }
        else if (strstr(command.c_str(), "CREATE"))
        {
            std::string name = "";
            int k = 0;
            while (command.c_str()[k] != '[')
                k++;
            k++;
            while (command.c_str()[k] != ']')
            {
                name = name + command.c_str()[k];
                k++;
            }
            
            if (hKey != NULL)
            {
                wchar_t wtext[200];
                mbstowcs(wtext, name.c_str(), strlen(name.c_str()) + 1);
                LPWSTR ptr = wtext;
                rooti++;
                roots[rooti] = hKey;
                int lResult = RegCreateKeyEx(roots[rooti], ptr, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
                RegCloseKey(hKey);
                hKey = roots[rooti];
                rooti--;
                if (lResult != ERROR_SUCCESS)
                {
                    if (lResult == ERROR_FILE_NOT_FOUND) {
                        printf("Key not found.\n");
                        return TRUE;
                    }
                    else {
                        printf("Error opening key.\n");
                        return FALSE;
                    }
                }
                printKeysValues(hKey);
            }
        }
        else if (strstr(command.c_str(), "FLAGREAD"))
        {

            std::string path = "reg flags HKEY_LOCAL_MACHINE\\";
            int k = 0;
            while (command.c_str()[k] != '[')
                k++;
            k++;
            while (command.c_str()[k] != ']')
            {
                path = path + command.c_str()[k];
                k++;
            }

            system(path.c_str());
        }
        else if (strstr(command.c_str(), "ADD"))
        {
         
            DWORD type = NULL;
            std::string namestr = "";
            std::string typestr = "";
            std::string datastr = "";
            int k = 0;
            while (command.c_str()[k] != '[')
                k++;
            k++;
            while (command.c_str()[k] != ']')
            {
                namestr = namestr + command.c_str()[k];
                k++;
            }
            k = k + 2;
            while (command.c_str()[k] != ']')
            {
                typestr = typestr + command.c_str()[k];
                k++;
            }
            k = k + 2;
            while (command.c_str()[k] != ']')
            {
                datastr = datastr + command.c_str()[k];
                k++;
            }
            BYTE* bytes = 0;
            DWORD size;
            wchar_t wtext[200];
            mbstowcs(wtext, namestr.c_str(), strlen(namestr.c_str()) + 1);
            LPWSTR ptr = wtext;
            if (typestr.compare("REG_BINARY") == 0)
            {
                type = REG_BINARY;
                int intbytes = 1;
                int binary[30];
                if (datastr.c_str()[0] != '0')
                {
                    for (int i = 1; i < strlen(datastr.c_str()); i++)
                    {
                        intbytes = intbytes * 2;
                        if (datastr.c_str()[i] == '1')
                            intbytes++;
                    }
                }
                size = strlen(datastr.c_str()) / 8 + 1;
                bytes = new BYTE[size];
                for (int i = 0; i < size; i++)
                    bytes[size - 1 - i] = (intbytes >> (i * 8));
            }
            else if (typestr.compare("REG_DWORD") == 0 || typestr.compare("REG_DWORD_LITTLE_ENDIAN")==0 || typestr.compare("REG_DWORD_BIG_ENDIAN") == 0)
            {
                type = REG_DWORD;
                long intbytes = _byteswap_ulong(std::stoi(datastr));
                size = 4;
                bytes = new BYTE[size];
                for (int i = 0; i < size; i++)
                    bytes[size - 1 - i] = (intbytes >> (i * 8));
            }
              

            else if (typestr.compare("REG_EXPAND_SZ") == 0)
            {
                type = REG_EXPAND_SZ;
                datastr = datastr;
                wchar_t wwtext[200];
                size = strlen(datastr.c_str()) * 2;
                mbstowcs(wwtext, datastr.c_str(), strlen(datastr.c_str()) + 1);
                LPWSTR wptr = wwtext;
                bytes = (LPBYTE)wptr;
            }

            else if (typestr.compare("REG_SZ") == 0)
            {
                type = REG_SZ;
                datastr = datastr;
                wchar_t wwtext[200];
                size = strlen(datastr.c_str())*2;
                mbstowcs(wwtext, datastr.c_str(), strlen(datastr.c_str()) + 1);
                LPWSTR wptr = wwtext;
                bytes = (LPBYTE)wptr;
            }
            else if (typestr.compare("REG_QWORD") == 0)
            {
                type = REG_QWORD;
                long long intbytes = _byteswap_uint64(std::stoll(datastr));
                size = 8;
                bytes = new BYTE[size];
                for (int i = 0; i < size; i++)
                    bytes[size - 1 - i] = (intbytes >> (i * 8));
            }
            if (hKey != NULL)
            {
                int lResult = RegSetValueEx(hKey, ptr, 0, type, bytes, size);
                if (lResult != ERROR_SUCCESS)
                {
                    if (lResult == ERROR_FILE_NOT_FOUND) {
                        printf("Key not found.\n");
                        return TRUE;
                    }
                    else {
                        printf("Error opening key.\n");
                        std::cout << lResult;
                        return FALSE;
                    }
                }
                printKeysValues(hKey);
            }
        }
        else if (strstr(command.c_str(), "INFORM"))
        {
        std::string isexit = "";
            HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            while (strcmp(isexit.c_str(), "") == 0)
            {
                LSTATUS lErrorCode = RegNotifyChangeKeyValue(hKey, TRUE, REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_ATTRIBUTES | REG_NOTIFY_CHANGE_LAST_SET | REG_NOTIFY_CHANGE_SECURITY, hEvent, TRUE);
                if (lErrorCode != ERROR_SUCCESS)
                {
                    std::cout << "Error in RegNotifyChangeKeyValue";
                    std::cout << lErrorCode;
                    std::cout << "\n";
                }
                std::cout << "Waiting for a change in the specified key...\n";
                if (WaitForSingleObject(hEvent, INFINITE) == WAIT_FAILED)
                {
                    std::cout << "Error in WaitForSingleObject";
                    std::cout << GetLastError();
                    std::cout << "\n";
                }
                else
                {
                    std::cin >> isexit;
                    std::cout << "Change has occurred\n";
                }
                printKeysValues(hKey);
            }
        }
        else if (strstr(command.c_str(), "SEARCH"))
        {
            rooti++;
            roots[rooti] = hKey;
            fl = false;
            std::string name = "";
            int k = 0;
            while (command.c_str()[k] != '[')
                k++;
            k++;
            while (command.c_str()[k] != ']')
            {
                name = name + command.c_str()[k];
                k++;
            }
            search(hKey, name);
            hKey = globalKey;
            printKeysValues(hKey);
        }

        std::cout << "\n";
        std::cin >> command;
        std::cout << "\n";
    }
}
