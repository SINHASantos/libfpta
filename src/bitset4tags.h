/*
 * Copyright 2016-2018 libfptu authors: please see AUTHORS file.
 *
 * This file is part of libfptu, aka "Fast Positive Tuples".
 *
 * libfptu is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libfptu is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libfptu.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "fast_positive/tuples_internal.h"

namespace fptu {

struct bitset4tags {
  /* std::bitset прекрасен, но требует инстанцирования под максимальный
   * размер. При этом (на стеке) будет выделено и заполнено нулями 4K,
   * в итоге расходы превысят экономию.
   *
   * Поэтому здесь реализована битовая карта, размер которой минимизируется
   * под актуальный разброс значений тегов полей. */

  struct minimize {
    typedef size_t unit;

    enum { unit_size = sizeof(unit), unit_bits = unit_size * 8 };

    unsigned lo_part;
    unsigned hi_part;
    unsigned blank;
    unsigned top;

    /* Оптимизация:
     *  - уменьшаем размер битовой карты опираясь на разброс значений тэгов;
     *  - для этого дизъюнкцией собираем значения тегов в битовой маске;
     *  - велика вероятность, что в кортеже нет массивов и не используется
     *    резерный бит, поэтому вычисляем сдвиг для пропуска этих нулей.
     *  - по старшему биту получаем верхний предел для размера битовой карты.
     */
    minimize(const fptu_field *const from, const fptu_field *const end,
             uint_fast16_t disjunction) {
      /* собираем в маску оставшиеся значения */
      for (auto i = from; i < end; ++i)
        if (likely(!i->is_dead()))
          disjunction |= i->tag;

      /* вполне вероятно, что резерный бит всегда нулевой, также возможно что
       * нет массивов, тогда размер карты (и трафик по памяти) можно сократить
       * в 2-4 раза. */
      blank = (disjunction & fptu_fr_mask)
                  ? 0u
                  : (unsigned)fptu_ct_reserve_bits +
                        ((disjunction & fptu_farray) ? 0u : 1u);
      lo_part = (1 << (fptu_typeid_bits + fptu_ct_reserve_bits)) - 1;
      hi_part = lo_part ^ UINT16_MAX;
      assert((lo_part >> blank) >= (disjunction & lo_part));
      top = (disjunction & lo_part) + ((disjunction & hi_part) >> blank) + 1;
    }

    size_t bytes() const {
      return ((top + unit_bits - 1) / unit_bits) * unit_size;
    }

    struct pair {
      size_t index;
      unit mask;

      pair(size_t i) : index(i / unit_bits), mask((unit)1 << (i % unit_bits)) {}
    };

    pair map(uint_fast16_t tag) const {
      assert(!fptu_tag_is_dead(tag));
      assert((lo_part >> blank) >= (tag & lo_part));
      tag = (tag & lo_part) + ((tag & hi_part) >> blank);
      assert(tag < top);
      return pair(tag);
    }
  };

  bitset4tags(const minimize &params, void *buffer)
      : bitset((minimize::unit *)buffer), params(params) {
    memset(bitset, 0, params.bytes());
  }

  void set(uint_fast16_t ct) {
    const auto i = params.map(ct);
    bitset[i.index] |= i.mask;
  }

  bool test(uint_fast16_t ct) const {
    const auto i = params.map(ct);
    return (bitset[i.index] & i.mask) != 0;
  }

  bool test_and_set(uint_fast16_t ct) {
    const auto i = params.map(ct);
    if (bitset[i.index] & i.mask)
      return true;
    bitset[i.index] |= i.mask;
    return false;
  }

protected:
  minimize::unit *const bitset;
  minimize params;
};

} // namespace fptu
