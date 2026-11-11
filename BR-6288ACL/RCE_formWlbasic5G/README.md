# EDIMAX BR-6288ACL /goform/formWlbasic5G command injection Vulnerability 
## Description

EDIMAX BR-6288ACL Firmware BR6288ACL_1.12 was discovered to contain a remote code execution (RCE) vulnerability via the `repeaterSSID` parameter in the `/goform/formWlbasic5G`. 

## EDIMAX BR-6288ACL version information

- Device：EDIMAX BR-6288ACL
- Firmware Version：BR6288ACL_1.12
- Manufacturer's website information：https://www.edimax.com/edimax/global/
- Firmware download address：https://www.edimax.com/edimax/download/download/data/edimax/global/download/home_legacy_wireless_routers/br-6288acl


## Vulnerability information

We can see that the  `v21` variable receives `repeaterSSID` parameter from a POST request. The statement `wlgetHiddenSSIDinfo5g(v21);` will call function `wlgetHiddenSSIDinfo5g`.Inside the function `wlgetHiddenSSIDinfo5g`, a1 receives the value of `v21` and passes it to `v20` in line 24. And the statement `sprintf(v19, "iwpriv wlan0 set_mib ssid2scan=\"%s\"", v20);` will pass `v20` to variable `v19`.  However, since the user can control the input of `repeaterSSID`, the statement `system(v19);` in line 27  can cause a command injection vulnerability. This vulnerability allows an attacker to execute arbitrary commands through the `repeaterSSID` parameter.

![1.png](imgs/1.png)
![12.png](imgs/1-2.png)

We use qemu-system to run the firmware.Then type `ls` command on the terminal of firmware.

![2.png](imgs/2.png)

We use BurpSuite to attck. We fill in information as shown in the figure below. And click the "Send" button. 

The purpose of `echo a > /b.txt`command is to output `a` to file `/b.txt` .

![3.png](imgs/3.png)

Then type `ls` and `cat /b.txt`  command on the terminal of firmware. We can see that `b.txt` was created and the the content of file `b.txt` is `a`.

![4.png](imgs/4.png)

## Vulnerability Assessment Score
CVSS Score: 9.8
Vector String： CVSS:4.0/AV:N/AC:L/AT:N/PR:N/UI:N/VC:H/VI:H/VA:H/SC:N/SI:N/SA:N
Reason: This vulnerability is a Pre-authentication Remote Code Execution vulnerability.It can execute any command.