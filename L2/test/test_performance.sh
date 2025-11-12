#!/bin/bash

echo -e "🧪 ANTI-BOT DETECTOR TEST"
echo -e "============================="

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Create logs directory
mkdir -p logs

# Compile
echo -e "Compiling mouse_simulator.c..."
gcc -pthread -lm -O2 -I ../include ../src/main.c ../src/functions.c -o antibot
if [ $? -ne 0 ]; then
    echo -e "${RED} Compilation failed${NC}"
    exit 1
fi

TOTAL_TESTS=0
PASSED_TESTS=0

echo -e "🚀 Running tests..."


# TEST 1: Verify pthread_create is used
echo -e "${YELLOW} 1. Code uses pthread_create${NC}"
PTHREAD_CREATE_COUNT=$(grep -c "pthread_create" ../src/main.c)
if [ "$PTHREAD_CREATE_COUNT" -ge 2 ]; then
    echo -e "    ${GREEN}PASSED - Found $PTHREAD_CREATE_COUNT calls to pthread_create${NC}"
    PASSED_TESTS=$((PASSED_TESTS + 1))
else
    echo -e "    ${RED}${RED}FAILED - Found only $PTHREAD_CREATE_COUNT calls (expected ≥2)${NC}"
fi
TOTAL_TESTS=$((TOTAL_TESTS + 1))


# TEST 2: Verify pthread_mutex_lock is used
echo -e "${YELLOW} 2. Code uses pthread_mutex_lock${NC}"
MUTEX_LOCK_COUNT=$(grep -c "pthread_mutex_lock" ../src/main.c)
if [ "$MUTEX_LOCK_COUNT" -ge 3 ]; then
    echo -e "    ${GREEN}PASSED - Found $MUTEX_LOCK_COUNT mutex locks${NC}"
    PASSED_TESTS=$((PASSED_TESTS + 1))
else
    echo -e "    ${RED}${RED}FAILED - Found only $MUTEX_LOCK_COUNT locks (expected ≥3)${NC}"
fi
TOTAL_TESTS=$((TOTAL_TESTS + 1))


# TEST 3: Verify pthread_mutex_unlock is used
echo -e "${YELLOW} 3. Code uses pthread_mutex_unlock${NC}"
MUTEX_UNLOCK_COUNT=$(grep -c "pthread_mutex_unlock" ../src/main.c)
if [ "$MUTEX_UNLOCK_COUNT" -ge 3 ]; then
    echo -e "    ${GREEN}PASSED - Found $MUTEX_UNLOCK_COUNT mutex unlocks${NC}"
    PASSED_TESTS=$((PASSED_TESTS + 1))
else
    echo -e "    ${RED}${RED}FAILED - Found only $MUTEX_UNLOCK_COUNT unlocks (expected ≥3)${NC}"
fi
TOTAL_TESTS=$((TOTAL_TESTS + 1))


# TEST 4: Verify pthread_join is used
echo -e "${YELLOW} 4. Code uses pthread_join${NC}"
PTHREAD_JOIN_COUNT=$(grep -c "pthread_join" ../src/main.c)
if [ "$PTHREAD_JOIN_COUNT" -ge 2 ]; then
    echo -e "    ${GREEN}PASSED - Found $PTHREAD_JOIN_COUNT calls to pthread_join${NC}"
    PASSED_TESTS=$((PASSED_TESTS + 1))
else
    echo -e "    ${RED}${RED}FAILED - Found only $PTHREAD_JOIN_COUNT joins (expected ≥2)${NC}"
fi
TOTAL_TESTS=$((TOTAL_TESTS + 1))


# TEST 5: Verify sleep/usleep is used
echo -e "${YELLOW} 5. Code uses sleep functions${NC}"
if grep -qE "(sleep\(|usleep\(|nanosleep\()" ../src/main.c; then
    echo -e "    ${GREEN}PASSED - Found sleep functions${NC}"
    PASSED_TESTS=$((PASSED_TESTS + 1))
else
    echo -e "    ${RED}${RED}FAILED - No sleep functions found${NC}"
fi
TOTAL_TESTS=$((TOTAL_TESTS + 1))


# TEST 6: Verify file/data reading
echo -e "${YELLOW} 6. Code reads data/simulates mouse events${NC}${NC}"
if grep -qE "(read\(|generate_.*_movement)" ../src/main.c; then
    echo -e "    ${GREEN}PASSED - Found data reading/generation${NC}"
    PASSED_TESTS=$((PASSED_TESTS + 1))
else
    echo -e "    ${RED}FAILED - No data reading found"
fi
TOTAL_TESTS=$((TOTAL_TESTS + 1))
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
echo -e "   Running antibot_detector in BOT mode..."
timeout 12 ./antibot mouse_bot.bin > logs/output_bot.log 2>&1


# TEST 7: Validate BOT detection
echo -e "${YELLOW} 7. Validate BOT detection${NC}"
if grep -q "ALERTA BOT DETECTADO" logs/output_bot.log; then
    echo -e "    ${GREEN}PASSED - Correctly detected as BOT${NC}"
    PASSED_TESTS=$((PASSED_TESTS + 1))
else
    echo -e "    ${RED}FAILED - Did not detect as BOT${NC}"
fi
TOTAL_TESTS=$((TOTAL_TESTS + 1))


# TEST 8: Validate variance calculation in BOT mode
echo -e "${YELLOW} 8. Variance calculated in BOT mode${NC}"
if grep -q "Varianza: 0.00 (umbral: 50.00)" logs/output_bot.log; then
    echo -e "    ${GREEN}PASSED - Variance is low ($VARIANCE < 50)${NC}"
    PASSED_TESTS=$((PASSED_TESTS + 1))

else
    echo -e "    ${RED}FAILED - No variance calculation found${NC}"
fi
TOTAL_TESTS=$((TOTAL_TESTS + 1))


# TEST 9: Validate BOT alerts count
echo -e "${YELLOW} 9. BOT alerts are counted${NC}"
BOT_ALERTS=$(grep -oP 'Alertas:\s*\K\d+' logs/output_bot.log | tail -1)
if [ ! -z "$BOT_ALERTS" ] && [ "$BOT_ALERTS" -gt 0 ]; then
    echo -e "    ${GREEN}PASSED - Found $BOT_ALERTS bot alerts${NC}"
    PASSED_TESTS=$((PASSED_TESTS + 1))
else
    echo -e "    ${RED}FAILED - No bot alerts found${NC}"
fi
TOTAL_TESTS=$((TOTAL_TESTS + 1))

#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
echo -e "   Running antibot_detector in HUMAN mode..."
timeout 12 ./antibot mouse_human.bin > logs/output_human.log 2>&1

# TEST 10: Validate HUMAN detection
echo -e "${YELLOW} 10. Validate HUMAN detection${NC}"
if grep -q "ALERTA BOT DETECTADO" logs/output_human.log; then
    echo -e "    ${RED}FAILED - Detect as BOT${NC}"
else
    echo -e "    ${GREEN}PASSED - Correct detection${NC}"
    PASSED_TESTS=$((PASSED_TESTS + 1))
fi
TOTAL_TESTS=$((TOTAL_TESTS + 1))

# ============================================
# Calculate percentage
# ============================================
PERCENTAGE=$((PASSED_TESTS * 100 / TOTAL_TESTS))

echo -e ""
echo -e "⚙️  FINAL RESULT:"
echo -e "==================="
echo -e "Total Tests: $TOTAL_TESTS"

# Final evaluation
if [ $PERCENTAGE -ge 95 ]; then
    echo -e "${GREEN} Tests passed: $PASSED_TESTS/$TOTAL_TESTS${NC}"
    echo -e "${GREEN} Percentage: $PERCENTAGE%${NC}"
    echo -e "${GREEN} EXCELLENT! Anti-bot detector works correctly${NC}"
    exit 0
elif [ $PERCENTAGE -ge 80 ]; then
    echo -e "${YELLOW} Tests passed: $PASSED_TESTS/$TOTAL_TESTS${NC}"
    echo -e "${YELLOW} Percentage: $PERCENTAGE%${NC}"
    echo -e "${YELLOW} GOOD - Some aspects need improvement${NC}"
    exit 1
else
    echo -e "${RED} Tests passed: $PASSED_TESTS/$TOTAL_TESTS${NC}"
    echo -e "${RED} Percentage: $PERCENTAGE%${NC}"
    echo -e "${RED} NEEDS WORK - Multiple tests failed${NC}"
    exit 2
fi
