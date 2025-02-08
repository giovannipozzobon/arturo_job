/**
 * @file       default.h
 *
 * @brief      CLI Command line headers
 *
 * @author     Paul Robson
 *
 * @date       08/02/2025
 *
 */

#pragma once

typedef void (*APPFUNCTION)(char *params);

bool CMDRunModules(char *cmd);
bool CMDCLICommand(char *cmd);
bool CMDExecute(char *cmd);