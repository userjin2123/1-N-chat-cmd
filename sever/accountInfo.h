/*�̸�: accountInfo/h
 * ����: Ŭ���̾�Ʈ ���� ���� ����ü
 */

#ifndef __ACCOUNTINFO_H__
#define __ACCOUNTINFO_H__

#define id_LEN     10
#define pass_LEN   30
#define nick_LEN  30

typedef struct __accountInfo
{
	char id[id_LEN];
	char password[pass_LEN];
	char nickname[nick_LEN];
} accInfo;

#endif
/* end of file */#pragma once
