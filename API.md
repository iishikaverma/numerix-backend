# Numerix API Documentation

Base URL:

http://localhost:8080

---

## 1. Bisection Method

### Endpoint

POST /api/solve/bisection

### Request

```json
{
  "a": 1,
  "b": 2,
  "tolerance": 0.0001
}