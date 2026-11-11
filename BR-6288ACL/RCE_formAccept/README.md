# EDIMAX BR-6288ACL /goform/formAccept command injection Vulnerability 
## Description

EDIMAX BR-6288ACL Firmware BR6288ACL_1.12 was discovered to contain a remote code execution (RCE) vulnerability via the `submit-url` parameter in the `/goform/formAccept`. 

## EDIMAX BR-6288ACL version information

- Device：EDIMAX BR-6288ACL
- Firmware Version：BR6288ACL_1.12
- Manufacturer's website information：https://www.edimax.com/edimax/global/
- Firmware download address：https://www.edimax.com/edimax/download/download/data/edimax/global/download/home_legacy_wireless_routers/br-6288acl

## Vulnerability information

We can see that the  `v4` variable receives `submit-url` parameter from a POST request. However, since the user can control the input of `submit-url`, the statement `system(v4);` in line 7  can cause a command injection vulnerability. This vulnerability allows an attacker to execute arbitrary commands through the `submit-url` parameter.

![1.png](imgs/1.png)

We use qemu-system to run the firmware.Then type `ls` command on the terminal of firmware.

![2.png](imgs/2.png)

We use BurpSuite to attck. We fill in information as shown in the figure below. And click the "Send" button. 

The purpose of `echo a > /2.txt`command is to output `a` to file `/2.txt` .

![3.png](imgs/3.png)

Then type `ls` and `cat /2.txt`  command on the terminal of firmware. We can see that `2.txt` was created and the the content of file `2.txt` is `a`.

![4.png](imgs/4.png)

## Vulnerability Assessment Score
CVSS Score: 9.8
Vector String： CVSS:4.0/AV:N/AC:L/AT:N/PR:N/UI:N/VC:H/VI:H/VA:H/SC:N/SI:N/SA:N
Reason: This vulnerability is a Pre-authentication Remote Code Execution vulnerability.It can execute any command.