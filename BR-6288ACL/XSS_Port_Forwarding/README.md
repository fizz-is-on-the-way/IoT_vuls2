# EDIMAX BR-6288ACL XSS Vulnerability (Access Control)
## Description

EDIMAX BR-6288ACL Firmware BR6288ACL_1.12 contains a Store Cross-site scripting (XSS) vulnerability in the `Access Control`  Page.

## EDIMAX BR-6288ACL version information

- Device：EDIMAX BR-6288ACL
- Firmware Version：BR6288ACL_1.12
- Manufacturer's website information：https://www.edimax.com/edimax/global/
- Firmware download address：https://www.edimax.com/edimax/download/download/data/edimax/global/download/home_legacy_wireless_routers/br-6288acl

## Vulnerability information

In the settings under the `Advanced` page, there is an option called `Port Forwarding`. There is a Store Cross-site scripting vulnerability in `Comment` input box. 

Firstly, we click the `Save Settings` button on the `Port Forwarding` page.

![1.png](imgs/1.png)

The page will check the value of `Comment`, but it does not check on the server, So we use BurpSuite to bypass. 

We fill in information as shown in the figure below. Then click the `Send` button to send the request.

![2.png](imgs/2.png)

Once the request is sent, we refresh the `Port Forwarding` page.Then the web site will execute the javascript we just inputted. This is a Store Cross-site scripting vulnerability, if someone else visits the page, the javascript will also be executed.

![3.png](imgs/3.png)

