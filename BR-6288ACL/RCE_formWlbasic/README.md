# EDIMAX BR-6288ACL /goform/formWlbasic command injection Vulnerability 
## Description

EDIMAX BR-6288ACL Firmware BR6288ACL_1.12 was discovered to contain a remote code execution (RCE) vulnerability via the `repeaterSSID` parameter in the `/goform/formWlbasic`. 

## EDIMAX BR-6288ACL version information

- Device：EDIMAX BR-6288ACL
- Firmware Version：BR6288ACL_1.12
- Manufacturer's website information：https://www.edimax.com/edimax/global/
- Firmware download address：https://www.edimax.com/edimax/download/download/data/edimax/global/download/home_legacy_wireless_routers/br-6288acl


## Vulnerability information

We can see that the  `v27` variable receives `repeaterSSID` parameter from a POST request. The statement `wlgetHiddenSSIDinfo(v27);` will call function `wlgetHiddenSSIDinfo`.Inside the function `wlgetHiddenSSIDinfo`, a1 receives the value of `v27` and passes it to `v21` in line 26. And the statement `sprintf(v20, "iwpriv wlan1 set_mib ssid2scan=\"%s\"", v21);` will pass `v21` to variable `v20`.  However, since the user can control the input of `repeaterSSID`, the statement `system(v20);` in line 29  can cause a command injection vulnerability. This vulnerability allows an attacker to execute arbitrary commands through the `repeaterSSID` parameter.

![1.png](imgs/1.png)
![12.png](imgs/1-2.png)

We use qemu-system to run the firmware.Then type `ls` command on the terminal of firmware.

![2.png](imgs/2.png)

We use BurpSuite to attck. We fill in information as shown in the figure below. And click the "Send" button. 

The purpose of `echo a > /a.txt`command is to output `a` to file `/a.txt` .

![3.png](imgs/3.png)

Then type `ls` and `cat /a.txt`  command on the terminal of firmware. We can see that `a.txt` was created and the the content of file `a.txt` is `a`.

![4.png](imgs/4.png)

## Vulnerability Assessment Score
CVSS Score: 9.8
Vector String： CVSS:4.0/AV:N/AC:L/AT:N/PR:N/UI:N/VC:H/VI:H/VA:H/SC:N/SI:N/SA:N
Reason: This vulnerability is a Pre-authentication Remote Code Execution vulnerability.It can execute any command.