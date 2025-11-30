## 🎯 Design Principles

### 1. **Modularity**
Setiap komponen terpisah dan independent:
- `point`, `size`, `rect` bisa dipakai standalone
- Event system bisa dipakai tanpa window
- Widget system terpisah dari rendering

### 2. **Zero-Cost Abstraction**
- Header-only templates
- Inline optimization
- No virtual dispatch (kecuali Widget)
- Compile-time type checking

### 3. **Type Safety**
- C++20 concepts untuk template constraints
- `std::variant` untuk type-safe unions
- No raw pointers dalam public API
- RAII everywhere

### 4. **Modern C++**
- C++23 features
- Spaceship operator (`<=>`)
- Concepts
- Ranges (future)
- Coroutines (future)

### 5. **Performance**
- Saturated arithmetic (no branches)
- Move semantics
- Perfect forwarding
- Cache-friendly layout